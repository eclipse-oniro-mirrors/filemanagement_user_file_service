/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
const pickerHelper = requireInternal('file.picker');

let gContext = undefined;

const PhotoViewMIMETypes = {
  IMAGE_TYPE: 'image/*',
  VIDEO_TYPE: 'video/*',
  IMAGE_VIDEO_TYPE: '*/*',
  INVALID_TYPE: ''
};

const DocumentSelectMode = {
  FILE: 0,
  FOLDER: 1,
  MIXED: 2,
};

const MergeTypeMode = {
  DEFAULT: 0,
  AUDIO: 1,
  VIDEO: 2,
  DOCUMENT: 3,
  PICTURE: 4,
};

const DocumentPickerMode = {
  DEFAULT: 0,
  DOWNLOAD: 1,
};

const ExtTypes = {
  DOWNLOAD_TYPE: 'filePicker',
  AUDIO_PICKER_TYPE: 'audioPicker',
  PHOTO_PICKER_TYPE: 'photoPicker',
};

const PickerDetailType = {
  FILE_MGR_AUTH: 'downloadAuth',
  FILE_MGR_SELECT:'select',
  FILE_MGR_SAVE:'save',
};

const ErrCode = {
  INVALID_ARGS: 13900020,
  RESULT_ERROR: 13900042,
  NAME_TOO_LONG: 13900030,
  CONTEXT_NO_EXIST: 16000011,
};

const ERRCODE_MAP = new Map([
  [ErrCode.INVALID_ARGS, 'Invalid argument'],
  [ErrCode.RESULT_ERROR, 'Unknown error'],
  [ErrCode.NAME_TOO_LONG, 'File name too long'],
  [ErrCode.CONTEXT_NO_EXIST, 'Current ability failed to obtain context'],
]);

const PHOTO_VIEW_MIME_TYPE_MAP = new Map([
  [PhotoViewMIMETypes.IMAGE_TYPE, 'FILTER_MEDIA_TYPE_IMAGE'],
  [PhotoViewMIMETypes.VIDEO_TYPE, 'FILTER_MEDIA_TYPE_VIDEO'],
  [PhotoViewMIMETypes.IMAGE_VIDEO_TYPE, 'FILTER_MEDIA_TYPE_ALL'],
]);

const ACTION = {
  SELECT_ACTION: 'ohos.want.action.OPEN_FILE',
  SELECT_ACTION_MODAL: 'ohos.want.action.OPEN_FILE_SERVICE',
  SAVE_ACTION: 'ohos.want.action.CREATE_FILE',
  SAVE_ACTION_MODAL: 'ohos.want.action.CREATE_FILE_SERVICE',
};

const CREATE_FILE_NAME_LENGTH_LIMIT = 256;
const ARGS_ZERO = 0;
const ARGS_ONE = 1;
const ARGS_TWO = 2;
const RESULT_CODE_ERROR = -1;
const RESULT_CODE_OK = 0;
const LENGTH_TWO = 2;
const LENGTH_THREE = 3;
let suffixIndex = -1;

/*
* UTF-8字符编码数值对应的存储长度：
* 0000 - 0x007F (eg: a~z A~Z 0~9）
* 0080 - 0x07FF (eg: 希腊字母）
* 0800 - 0xFFFF (eg: 中文）
* 其他 (eg: 平面符号）
*/
function strSizeUTF8(str) {
  let strLen = str.length;
  let bytesLen = 0;
  let greeceLen = 2;
  let chineseLen = 3;
  let othersLen = 4;
  for (let i = 0; i < strLen; i++) {
    let charCode = str.charCodeAt(i);
    if (charCode <= 0x007f) {
      bytesLen++;
    } else if (charCode <= 0x07ff) {
      bytesLen += greeceLen;
    } else if (charCode <= 0xffff) {
      bytesLen += chineseLen;
    } else {
      bytesLen += othersLen;
    }
  }
  return bytesLen;
}

function checkArguments(args) {
  let checkArgumentsResult = undefined;
  if (args.length === ARGS_TWO && typeof args[ARGS_ONE] !== 'function') {
    checkArgumentsResult = getErr(ErrCode.INVALID_ARGS);
  }

  if (args.length > 0 && typeof args[ARGS_ZERO] === 'object') {
    let option = args[ARGS_ZERO];
    if (option.maxSelectNumber !== undefined) {
      if (option.maxSelectNumber.toString().indexOf('.') !== -1) {
        checkArgumentsResult = getErr(ErrCode.INVALID_ARGS);
      }
    }

    if (option.newFileNames === undefined || option.newFileNames.length <= 0) {
      return checkArgumentsResult;
    }

    for (let i = 0; i < option.newFileNames.length; i++) {
      let value = option.newFileNames[i];
      if (strSizeUTF8(value) >= CREATE_FILE_NAME_LENGTH_LIMIT) {
        console.log('[picker] checkArguments Invalid name: ' + value);
        checkArgumentsResult = getErr(ErrCode.NAME_TOO_LONG);
      }
    }
  }

  return checkArgumentsResult;
}

function getErr(errCode) {
  return {code: errCode, message: ERRCODE_MAP.get(errCode)};
}

function parsePhotoPickerSelectOption(args) {
  let config = {
    action: 'ohos.want.action.photoPicker',
    type: 'multipleselect',
    parameters: {
      uri: 'multipleselect',
      extType: ExtTypes.PHOTO_PICKER_TYPE,
    },
  };
  console.log('[picker] parse Photo SelectOption start ');
  if (args.length > ARGS_ZERO && typeof args[ARGS_ZERO] === 'object') {
    let option = args[ARGS_ZERO];
    if (option.maxSelectNumber && option.maxSelectNumber > 0) {
      let select = (option.maxSelectNumber === 1) ? 'singleselect' : 'multipleselect';
      config.type = select;
      config.parameters.uri = select;
      config.parameters.maxSelectCount = option.maxSelectNumber;
    }
    if (option.MIMEType && PHOTO_VIEW_MIME_TYPE_MAP.has(option.MIMEType)) {
      config.parameters.filterMediaType = PHOTO_VIEW_MIME_TYPE_MAP.get(option.MIMEType);
    }
  }
  console.log('[picker] parse Photo SelectOption end ');
  return config;
}

function anonymousPathArray(geturi) {
  let anonymousPathArrays = [];
  let anonymousPath = '';
  if (geturi === undefined) {
    return anonymousPathArrays;
  }
  for (let i = 0; i < geturi.length; ++i) {
    let lastSlashIndex = geturi[i].lastIndexOf('/');
    if (lastSlashIndex === -1) {
      anonymousPathArrays.push(geturi[i]);
    } else {
      let dirPath = geturi[i].substring(0, lastSlashIndex + 1);
      let fileName = geturi[i].substring(lastSlashIndex + 1);
      if (fileName.length <= 0) {
        anonymousPath = '******';
      } else {
        let lastLetter = fileName.slice(-1);
        let maskedName = '******' + lastLetter;
        anonymousPath = dirPath + maskedName;
      }
      anonymousPathArrays.push(anonymousPath);
    }
    }
  return anonymousPathArrays;
}

function getPhotoPickerSelectResult(args) {
  let selectResult = {
    error: undefined,
    data: undefined,
  };

  if (args.resultCode === 0) {
    let uris = args.photoUris;
    if (uris === undefined) {
      console.log('[picker] Photo uris is undefined');
      uris = [];
    }
    let isOriginal = args.isOriginal;
    selectResult.data = new PhotoSelectResult(uris, isOriginal);
  } else if (args.resultCode === -1) {
    selectResult.data = new PhotoSelectResult([], undefined);
  } else {
    selectResult.error = getErr(ErrCode.RESULT_ERROR);
  }
  console.log('[picker] photo selectResult end');
  return selectResult;
}

async function photoPickerSelect(...args) {
  if (args.length !== LENGTH_TWO) {
    console.error('[picker] args error: ' + args.length);
    return undefined;
  }
  let checkPhotoArgsResult = checkArguments(args[ARGS_ZERO]);
  if (checkPhotoArgsResult !== undefined) {
    console.log('[picker] Photo Invalid argument');
    throw checkPhotoArgsResult;
  }

  const config = parsePhotoPickerSelectOption(args[ARGS_ZERO]);

  let photoSelectContext = undefined;
  let photoSelectWindow = undefined;
  try {
    if (args[ARGS_ONE] !== undefined) {
      photoSelectContext = args[ARGS_ONE];
    } else {
      photoSelectContext = getContext(this);
    }
  } catch (getContextError) {
    console.error('[picker] getContext error: ' + getContextError);
    throw getErr(ErrCode.CONTEXT_NO_EXIST);
  }
  try {
    if (photoSelectContext === undefined) {
      console.error('[picker] photoSelectContext == undefined');
      throw getErr(ErrCode.CONTEXT_NO_EXIST);
    }
    let modalSelectResult = await modalPicker(photoSelectContext, config, photoSelectWindow);
    console.log('[picker] photo modalSelectResult is ready');
    const photoSelectResult = getPhotoPickerSelectResult(modalSelectResult);
    let inputArgs = args[ARGS_ZERO];
    if (inputArgs.length === ARGS_TWO && typeof inputArgs[ARGS_ONE] === 'function') {
      return inputArgs[ARGS_ONE](photoSelectResult.error, photoSelectResult.data);
    } else if (inputArgs.length === ARGS_ONE && typeof inputArgs[ARGS_ZERO] === 'function') {
      return inputArgs[ARGS_ZERO](photoSelectResult.error, photoSelectResult.data);
    }
    return new Promise((resolve, reject) => {
      if (photoSelectResult.data !== undefined) {
        resolve(photoSelectResult.data);
      } else {
        reject(photoSelectResult.error);
      }
    });
  } catch (error) {
    console.error('[picker] photo select error: ' + error);
  }
  return undefined;
}

function parseDocumentPickerSelectOption(args, action) {
  let config = {
    action: action,
    parameters: {
      startMode: 'choose',
      extType: ExtTypes.DOWNLOAD_TYPE,
      pickerType: PickerDetailType.FILE_MGR_SELECT,
    }
  };

  if (args.length > ARGS_ZERO && typeof args[ARGS_ZERO] === 'object') {
    let option = args[ARGS_ZERO];
    console.log('[picker] parseDocumentPickerSelectOption start');  
    config.parameters.key_select_mode = option.selectMode;
    config.parameters.key_merge_type_mode = option.mergeMode;
    config.parameters.key_is_encryption_supported = option.isEncryptionSupported;
    config.parameters.key_theme_color_mode = option.themeColor;
    if ((option.maxSelectNumber !== undefined) && option.maxSelectNumber > 0) {
      config.parameters.key_pick_num = option.maxSelectNumber;
    }
    if (option.defaultFilePathUri !== undefined) {
      config.parameters.key_pick_dir_path = option.defaultFilePathUri;
    }
    if ((option.fileSuffixFilters !== undefined) && option.fileSuffixFilters.length > 0) {
      config.parameters.key_file_suffix_filter = option.fileSuffixFilters;
    }
    if (option.authMode !== undefined) {
      config.parameters.key_auth_mode = option.authMode;
    }
    config.parameters.key_mult_auth_mode = option.multiAuthMode;
    if (option.multiUriArray !== undefined) {
      config.parameters.key_mult_uri_arr = option.multiUriArray;
      console.log('[picker] parseDocumentPickerSelectOption multiUriArray length: ' + option.multiUriArray.length);
    }
  }
  console.log('[picker] parseDocumentPickerSelectOption end'); 
  return config;
}

function parseAudioPickerSelectOption(args, action) {
  let config = {
    action: action,
    parameters: {
      extType: ExtTypes.AUDIO_PICKER_TYPE,
    }
  };
  if (args.length > ARGS_ZERO && typeof args[ARGS_ZERO] === 'object') {
    let option = args[ARGS_ZERO];
    if ((option.maxSelectNumber !== undefined) && option.maxSelectNumber > 0) {
      config.parameters.key_pick_num = option.maxSelectNumber;
    }
  }
  console.log('[picker] audio select config: ' + JSON.stringify(config));
  return config;
}

function getDocumentPickerSelectResult(args) {
  let selectResult = {
    error: undefined,
    data: undefined
  };
  if (args === undefined || args.resultCode === undefined) {
    selectResult.error = getErr(ErrCode.RESULT_ERROR);
    console.log('[picker] document select selectResult: ' + JSON.stringify(selectResult));
    return selectResult;
  }
  if (args.resultCode === RESULT_CODE_OK) {
    if (args.ability_params_stream) {
      selectResult.data = args.ability_params_stream;
      selectResult.error = args.resultCode;
    } else if (args.ability_params_udkey) {
      selectResult.data = args.ability_params_udkey;
      selectResult.error = args.resultCode;
    } else {
      selectResult.data = [];
      selectResult.error = RESULT_CODE_ERROR;
    }
  } else if (args.resultCode === RESULT_CODE_ERROR) {
    selectResult.data = [];
    selectResult.error = args.resultCode;
  }

  console.log('[picker] document select selectResult: : errorcode is = ' + selectResult.error +
              ', selecturi is = ' + anonymousPathArray(selectResult.data));
  return selectResult;
}

async function documentPickerSelect(...args) {
  if (args.length !== LENGTH_THREE) {
    console.error('[picker] args error: ' + args.length);
    return undefined;
  }
  let checkDocumentSelectArgsResult = checkArguments(args[ARGS_ZERO]);
  if (checkDocumentSelectArgsResult !== undefined) {
    console.log('[picker] Document Select Invalid argument');
    throw checkDocumentSelectArgsResult;
  }

  let documentSelectContext = undefined;
  let documentSelectConfig = undefined;
  let documentSelectResult = undefined;
  let selectResult = undefined;
  let documentSelectWindow = undefined;

  try {
    if (args[ARGS_ONE] !== undefined) {
      documentSelectContext = args[ARGS_ONE];
    } else {
      documentSelectContext = getContext(this);
    }
  } catch (getContextError) {
    console.error('[picker] getContext error: ' + getContextError);
    throw getErr(ErrCode.CONTEXT_NO_EXIST);
  }
  try {
    if (documentSelectContext === undefined) {
      console.error('[picker] documentSelectContext == undefined');
      throw getErr(ErrCode.CONTEXT_NO_EXIST);
    }
    if (args[ARGS_TWO] !== undefined) {
        documentSelectWindow = args[ARGS_TWO];
    }
    documentSelectConfig = parseDocumentPickerSelectOption(args[ARGS_ZERO], ACTION.SELECT_ACTION_MODAL);
    documentSelectResult = await modalPicker(documentSelectContext, documentSelectConfig, documentSelectWindow);
  } catch (paramError) {
    console.error('[picker] DocumentSelect paramError: ' + JSON.stringify(paramError));
  }
  selectResult = getDocumentPickerSelectResult(documentSelectResult);
  return sendResult(args[ARGS_ZERO], selectResult);
}

function parseDocumentPickerSaveOption(args, action) {
  let config = {
    action: action,
    parameters: {
      startMode: 'save',
      pickerMode: DocumentPickerMode.DEFAULT,
      extType: ExtTypes.DOWNLOAD_TYPE,
      pickerType: PickerDetailType.FILE_MGR_SAVE,
    }
  };
  console.log('[picker] parse document save option start');
  if (args.length > ARGS_ZERO && typeof args[ARGS_ZERO] === 'object') {
    let option = args[ARGS_ZERO];
    config.parameters.key_theme_color_mode = option.themeColor;
    if ((option.newFileNames !== undefined) && option.newFileNames.length > 0) {
      config.parameters.key_pick_file_name = option.newFileNames;
      config.parameters.saveFile = option.newFileNames[0];
    }

    if (option.defaultFilePathUri !== undefined) {
      config.parameters.key_pick_dir_path = option.defaultFilePathUri;
    }
    if ((option.fileSuffixChoices !== undefined) && option.fileSuffixChoices.length > 0) {
      config.parameters.key_file_suffix_choices = option.fileSuffixChoices;
    }
    if (option.pickerMode === DocumentPickerMode.DOWNLOAD) {
      config.parameters.pickerMode = option.pickerMode;
      config.parameters.pickerType = PickerDetailType.FILE_MGR_AUTH;
    }
  }
  console.log('[picker] document save config pickerMode is ' + config.parameters.pickerMode + 
    ',pickerType is ' + config.parameters.pickerType);
  return config;
}

function getAudioPickerSelectResult(args) {
  let selectResult = {
    error: undefined,
    data: undefined
  };
  if (args === undefined || args.resultCode === undefined) {
    selectResult.error = getErr(ErrCode.RESULT_ERROR);
    console.log('[picker] getAudioPickerSelectResult selectResult: ' + JSON.stringify(selectResult));
    return selectResult;
  }
  if (args.resultCode === RESULT_CODE_OK) {
    if (args.uriArr) {
      selectResult.data = args.uriArr;
      selectResult.error = args.resultCode;
    } else {
      selectResult.data = [];
      selectResult.error = args.resultCode;
    }
  } else if (args.resultCode === RESULT_CODE_ERROR) {
    selectResult.data = [];
    selectResult.error = args.resultCode;
  }

  console.log('[picker] getAudioPickerSelectResult selectResult: errorcode is = ' + selectResult.error +
              ', selecturi is = ' + anonymousPathArray(selectResult.data));
  return selectResult;
}


function getDocumentPickerSaveResult(args) {
  let saveResult = {
    error: undefined,
    data: undefined,
    suffix: -1
  };
  if (args === undefined || args.resultCode === undefined) {
    saveResult.error = getErr(ErrCode.RESULT_ERROR);
    console.log('[picker] getDocumentPickerSaveResult saveResult: ' + JSON.stringify(saveResult));
    return saveResult;
  }
  if (args.resultCode === RESULT_CODE_OK) {
    if (args.ability_params_stream) {
      saveResult.data = args.ability_params_stream;
      saveResult.error = args.resultCode;
      if (args.userSuffixIndex >= 0) {
        saveResult.suffix = args.userSuffixIndex;
      }
    } else {
      saveResult.data = [];
      saveResult.error = RESULT_CODE_ERROR;
    }
  } else if (args.resultCode === RESULT_CODE_ERROR) {
    saveResult.data = [];
    saveResult.error = args.resultCode;
  }

  console.log('[picker] getDocumentPickerSaveResult saveResult: errorcode is = ' + saveResult.error +
              ', selecturi is = ' + anonymousPathArray(saveResult.data) + ', usersavesuffix = ' + saveResult.suffix);
  return saveResult;
}

function startModalPicker(context, config, window) {
  if (context === undefined) {
    throw Error('[picker] Context undefined.');
  }
  if (config === undefined) {
    throw Error('[picker] Config undefined.');
  }
  gContext = context;
  if (pickerHelper === undefined) {
    throw Error('[picker] PickerHelper undefined.');
  }
  let helper;
  if (window !== undefined) {
    helper = pickerHelper.startModalPicker(gContext, config, window);
  } else {
    helper = pickerHelper.startModalPicker(gContext, config);
  }
  if (helper === undefined) {
    throw Error('[picker] Please check the parameter you entered.');
  }
  return helper;
}

async function modalPicker(context, config, window) {
  try {
    console.log('[picker] modalPicker start ');
    let modalResult = await startModalPicker(context, config, window);
    return modalResult;
  } catch (resultError) {
    console.error('[picker] Result error: ' + resultError);
    return undefined;
  }
}

async function documentPickerSave(...args) {
  if (args.length < LENGTH_TWO || args.length > LENGTH_THREE) {
    console.error('[picker] args error: ' + args.length);
    return undefined;
  }
  let checkDocumentSaveArgsResult = checkArguments(args[ARGS_ZERO]);
  if (checkDocumentSaveArgsResult !== undefined) {
    console.log('[picker] Document Save Invalid argument');
    throw checkDocumentSaveArgsResult;
  }

  let documentSaveContext = undefined;
  let documentSaveConfig = undefined;
  let documentSaveResult = undefined;
  let saveResult = undefined;
  let documentSaveWindow = undefined;

  try {
    if (args[ARGS_ONE] !== undefined) {
      documentSaveContext = args[ARGS_ONE];
    } else {
      documentSaveContext = getContext(this);
    }
  } catch (getContextError) {
    console.error('[picker] getContext error: ' + getContextError);
    throw getErr(ErrCode.CONTEXT_NO_EXIST);
  }
  if (args.length === LENGTH_THREE && args[ARGS_TWO] !== undefined) {
      documentSaveWindow = args[ARGS_TWO];
  }

  documentSaveConfig = parseDocumentPickerSaveOption(args[ARGS_ZERO], ACTION.SAVE_ACTION_MODAL);
  console.log('[picker] document save start');

  documentSaveResult = await modalPicker(documentSaveContext, documentSaveConfig, documentSaveWindow);
  saveResult = getDocumentPickerSaveResult(documentSaveResult);
  suffixIndex = saveResult.suffix;
  return sendResult(args[ARGS_ZERO], saveResult);
}

function getSelectedSuffixIndex() {
  console.log('[picker] Get Selected Suffix Index start');
  let index = suffixIndex;
  suffixIndex = -1;
  console.log('[picker] Get Selected Suffix Index end: ' + index);
  return index;
}

async function sendResult(args, result) {
  try {
    if (result === undefined) {
      console.log('[picker] result is undefined.');
      return undefined;
    }
    if (args.length === ARGS_TWO && typeof args[ARGS_ONE] === 'function') {
      console.log('[picker] sendresult is callback.');
      return args[ARGS_ONE](result.error, result.data);
    } else if (args.length === ARGS_ONE && typeof args[ARGS_ZERO] === 'function') {
      console.log('[picker] sendresult is callback without options');
      return args[ARGS_ZERO](result.error, result.data);
    }
    return new Promise((resolve, reject) => {
      if (result.data !== undefined) {
        resolve(result.data);
      } else {
        reject(result.error);
      }
    });
  } catch (resultError) {
    console.error('[picker] Result error: ' + resultError);
  }
  return undefined;
}

async function audioPickerSelect(...args) {
  if (args.length !== LENGTH_TWO) {
    console.error('[picker] args error: ' + args.length);
    return undefined;
  }
  let checkAudioArgsResult = checkArguments(args[ARGS_ZERO]);
  if (checkAudioArgsResult !== undefined) {
    console.log('[picker] Audio Invalid argument');
    throw checkAudioArgsResult;
  }

  const audioSelectConfig = parseAudioPickerSelectOption(args[ARGS_ZERO], ACTION.SELECT_ACTION);
  console.log('[picker] audio select config: ' + JSON.stringify(audioSelectConfig));

  let audioSelectContext = undefined;
  let audipSelectWindow = undefined;
  try {
    if (args[ARGS_ONE] !== undefined) {
      audioSelectContext = args[ARGS_ONE];
    } else {
      audioSelectContext = getContext(this);
    }
  } catch (getContextError) {
    console.error('[picker] getContext error: ' + getContextError);
    throw getErr(ErrCode.CONTEXT_NO_EXIST);
  }
  try {
    if (audioSelectContext === undefined) {
      console.error('[picker] audioSelectContext == undefined');
      throw getErr(ErrCode.CONTEXT_NO_EXIST);
    }
    let modalSelectResult = await modalPicker(audioSelectContext, audioSelectConfig, audipSelectWindow);
    let saveResult = getAudioPickerSelectResult(modalSelectResult);
    return sendResult(args[ARGS_ZERO], saveResult);
  } catch (error) {
    console.error('[picker] audio select error: ' + error);
  }
  return undefined;
}

class PhotoSelectOptions {
  constructor() {
    this.MIMEType = PhotoViewMIMETypes.INVALID_TYPE;
    this.maxSelectNumber = -1;
  }
}

class PhotoSelectResult {
  constructor(uris, isOriginalPhoto) {
    this.photoUris = uris;
    this.isOriginalPhoto = isOriginalPhoto;
  }
}

class PhotoSaveOptions {
  constructor() {
    this.newFileNames = undefined;
  }
}

class DocumentSelectOptions {
  constructor() {
    this.defaultFilePathUri = undefined;
    this.fileSuffixFilters = undefined;
    this.maxSelectNumber = undefined;
    this.authMode = false;
    this.selectMode = DocumentSelectMode.FILE;
    this.mergeMode = MergeTypeMode.DEFAULT;
    this.multiAuthMode = false;
    this.multiUriArray = undefined;
    this.isEncryptionSupported = false;
    this.themeColor = undefined;
  }
}

class DocumentSaveOptions {
  constructor() {
    this.newFileNames = undefined;
    this.defaultFilePathUri = undefined;
    this.fileSuffixChoices = undefined;
    this.pickerMode = DocumentPickerMode.DEFAULT;
    this.themeColor = undefined;
  }
}

class AudioSelectOptions {
  constructor() {}
}

class AudioSaveOptions {
  constructor() {
    this.newFileNames = undefined;
  }
}

function ParseContext(args)
{
  if (args.length > ARGS_TWO || args.length < ARGS_ZERO || typeof args[ARGS_ZERO] !== 'object') {
    return undefined;
  }
  return args[ARGS_ZERO];
}

function parseWindow(args)
{
  if (args.length !== ARGS_TWO) {
    console.log('[picker] ParseWindow: not window mode.');
    return undefined;
  }
  if (args.length === ARGS_TWO && typeof args[ARGS_ONE] !== 'object') {
    console.log('[picker] ParseWindow: not window mode or type err.');
    return undefined;
  }
  console.log('[picker] ParseWindow: window mode.');
  return args[ARGS_ONE]; 
}

class PhotoViewPicker {
  constructor(...args) {
    this.context = ParseContext(args);
  }
  select(...args) {
    return photoPickerSelect(args, this.context);
  }
  save(...args) {
    return documentPickerSave(args, this.context);
  }
}

class DocumentViewPicker {
  constructor(...args) {
    this.context = ParseContext(args);
    this.window = parseWindow(args);
  }
  select(...args) {
    return documentPickerSelect(args, this.context, this.window);
  }
  save(...args) {
    return documentPickerSave(args, this.context, this.window);
  }
  getSelectedIndex() {
    return getSelectedSuffixIndex();
  }
}

class AudioViewPicker {
  constructor(...args) {
    this.context = ParseContext(args);
  }
  select(...args) {
    return audioPickerSelect(args, this.context);
  }
  save(...args) {
    return documentPickerSave(args, this.context);
  }
}

export default {
  getSelectedSuffixIndex,
  startModalPicker,
  ExtTypes : ExtTypes,
  PickerDetailType: PickerDetailType,
  PhotoViewMIMETypes : PhotoViewMIMETypes,
  PhotoSelectOptions : PhotoSelectOptions,
  PhotoSelectResult : PhotoSelectResult,
  PhotoSaveOptions : PhotoSaveOptions,
  DocumentSelectMode : DocumentSelectMode,
  DocumentPickerMode : DocumentPickerMode,
  MergeTypeMode : MergeTypeMode,
  DocumentSelectOptions : DocumentSelectOptions,
  DocumentSaveOptions : DocumentSaveOptions,
  AudioSelectOptions : AudioSelectOptions,
  AudioSaveOptions : AudioSaveOptions,
  PhotoViewPicker : PhotoViewPicker,
  DocumentViewPicker: DocumentViewPicker,
  AudioViewPicker : AudioViewPicker,
};
