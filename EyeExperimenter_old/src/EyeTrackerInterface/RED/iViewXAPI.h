/**
 * iViewX API Documentation
 *
 * The iView X SDK provides an interface for communication between your 
 * application and the iView eye tracking server. The iView X SDK provides 
 * a large set of functions to control SMI eye trackers.
 * It supports a growing number of programming languages and environments,
 * e.g. C/C++, .Net, Matlab, Visual Basic, E-Prime.
 *
 * @author SMI GmbH
 */

/**
 * @file iViewXAPI.h
 *
 * @brief The file contains the prototype declarations for all supported
 * functions and data structs the customer can use to interact with
 * SMI eye tracking devices.
 **/

#pragma once

/* Microsoft VC++ */
#if defined (_MSC_VER)
#	if !defined (CALLBACK)
#		define CALLBACK __stdcall
#	endif
#	define DEPRECATED __declspec(deprecated)
#	if defined (__cplusplus)
#		define DLL_DEC_PRE extern "C" __declspec(dllimport)
#	else
#		define DLL_DEC_PRE extern __declspec(dllimport)
#	endif
#	define DLL_DEC_POST __stdcall

/* MinGW (GCC compiler on Windows) */
#elif defined (__MINGW32__)
#	if !defined (CALLBACK)
#		define CALLBACK __stdcall
#	endif
#	define DEPRECATED
#	define DLL_DEC_PRE extern "C" __stdcall
#	define DLL_DEC_POST

/* everthing else, e.g. Linux */
#else
#	if !defined (CALLBACK)
#		define CALLBACK
#	endif
#	define DEPRECATED
#	define DLL_DEC_PRE extern "C"
#	define DLL_DEC_POST
#endif

/**
 * @name ReturnValues
 * The ReturnValues list defines all possible return codes which can be returned
 * by the API functions.
 * @{
 */
#define RET_SUCCESS													1
#define RET_NO_VALID_DATA											2
#define RET_CALIBRATION_ABORTED										3
#define RET_SERVER_IS_RUNNING										4
#define RET_CALIBRATION_NOT_IN_PROGRESS								5
#define RET_WINDOW_IS_OPEN											11
#define RET_WINDOW_IS_CLOSED										12

#define ERR_COULD_NOT_CONNECT										100
#define ERR_NOT_CONNECTED											101
#define ERR_NOT_CALIBRATED											102
#define ERR_NOT_VALIDATED											103
#define ERR_EYETRACKING_APPLICATION_NOT_RUNNING						104
#define ERR_WRONG_COMMUNICATION_PARAMETER							105
#define ERR_WRONG_DEVICE											111
#define ERR_WRONG_PARAMETER											112
#define ERR_WRONG_CALIBRATION_METHOD								113
#define ERR_CALIBRATION_TIMEOUT										114
#define ERR_TRACKING_NOT_STABLE										115
#define ERR_INSUFFICIENT_BUFFER_SIZE								116
#define ERR_CREATE_SOCKET											121
#define ERR_CONNECT_SOCKET											122
#define ERR_BIND_SOCKET												123
#define ERR_DELETE_SOCKET											124
#define ERR_NO_RESPONSE_FROM_IVIEWX									131
#define ERR_INVALID_IVIEWX_VERSION									132
#define ERR_WRONG_IVIEWX_VERSION									133
#define ERR_ACCESS_TO_FILE											171
#define ERR_SOCKET_CONNECTION										181
#define ERR_EMPTY_DATA_BUFFER										191
#define ERR_RECORDING_DATA_BUFFER									192
#define ERR_FULL_DATA_BUFFER										193
#define ERR_IVIEWX_IS_NOT_READY										194
#define ERR_PAUSED_DATA_BUFFER										195
#define ERR_IVIEWX_NOT_FOUND										201
#define ERR_IVIEWX_PATH_NOT_FOUND									202
#define ERR_IVIEWX_ACCESS_DENIED									203
#define ERR_IVIEWX_ACCESS_INCOMPLETE								204
#define ERR_IVIEWX_OUT_OF_MEMORY									205
#define ERR_MULTIPLE_DEVICES										206
#define ERR_CAMERA_NOT_FOUND										211
#define ERR_WRONG_CAMERA											212
#define ERR_WRONG_CAMERA_PORT										213
#define ERR_USB2_CAMERA_PORT										214
#define ERR_USB3_CAMERA_PORT										215
#define ERR_COULD_NOT_OPEN_PORT										220
#define ERR_COULD_NOT_CLOSE_PORT									221
#define ERR_AOI_ACCESS												222
#define ERR_AOI_NOT_DEFINED											223
#define ERR_FEATURE_NOT_LICENSED									250
#define ERR_LICENSE_EXPIRED											251
#define ERR_DEPRECATED_FUNCTION										300
#define ERR_INITIALIZATION											400
#define ERR_FUNC_NOT_LOADED											401
/** @} */

/**
 * @name LogLevel
 * With these defines it is possible to setup the logging status
 * for the function @ref iV_Log. With @ref iV_Log it is possible to observe the
 * communication between a user’s application and iView X and/or function
 * calls. Log levels can be combined (e.g. LOG_BUG | LOG_IV_COMMAND | LOG_ETCOM).
 * @{
 */
#define LOG_LEVEL_BUG					1
#define LOG_LEVEL_iV_FCT				2
#define LOG_LEVEL_ALL_FCT				4
#define LOG_LEVEL_IV_COMMAND			8
#define LOG_LEVEL_RECV_IV_COMMAND		16
/** @} */

/**
 * @name EyeTrackingParameter
 * With ET_PARAM_ and function @ref iV_SetTrackingParameter it is possible
 * to change iView X and iView eye tracking server tracking parameters, for example
 * pupil threshold and corneal reflex thresholds, eye image contours, and other
 * parameters.
 *
 * Important note: This function can strongly affect tracking stability of
 * your iView X and iView eye tracking server system. Only experienced users should use
 * this function.
 * @{
 */
#define ET_PARAM_EYE_LEFT				0
#define ET_PARAM_EYE_RIGHT				1
#define ET_PARAM_EYE_BOTH				2

#define ET_PARAM_PUPIL_THRESHOLD		0
#define ET_PARAM_REFLEX_THRESHOLD		1
#define ET_PARAM_SHOW_AOI				2
#define ET_PARAM_SHOW_CONTOUR			3
#define ET_PARAM_SHOW_PUPIL				4
#define ET_PARAM_SHOW_REFLEX			5
#define ET_PARAM_DYNAMIC_THRESHOLD		6

#define ET_PARAM_PUPIL_AREA				11
#define ET_PARAM_PUPIL_PERIMETER		12
#define ET_PARAM_PUPIL_DENSITY			13
#define ET_PARAM_REFLEX_PERIMETER		14
#define ET_PARAM_REFLEX_PUPIL_DISTANCE	15
#define ET_PARAM_MONOCULAR				16
#define ET_PARAM_SMARTBINOCULAR			17
#define ET_PARAM_BINOCULAR				18
#define ET_PARAM_SMARTTRACKING			19

/** @} */

/**
 * @name CalibrationMethod
 * @{
 */

/**
 * An option flag to activate the smart calibration feature in iViewRED 4.2 or later eye tracking server. Add this flag to 
 * the @ref CalibrationStruct::method field when setting calibration parameters with @ref iV_SetupCalibration.
 */
#define CALIBRATIONMETHOD_SMARTCALIBRATION 0x80000000

/**
 * A bit mask to retrieve the real calibration method without advanced option flags e.g.
 * smart calibration.
 */
#define CALIBRATIONMETHOD_MASK 0x0000FFFF
/** @} */

/** @addtogroup enums Enumerations
 *  @{
 */

/**
 * @enum ETDevice
 *
 * @brief The enumeration ETDevice can be used in connection with
 * @ref iV_GetSystemInfo to get information about which type of device is
 * connected to iView X or iView eye tracking server. It is part of the @ref SystemInfoStruct.
 */
enum ETDevice {
	//! if no device is set up while running iView X application
	NONE = 0,

	//! iView X based remote eye tracking devices
	RED = 1,

	//! eye tracking server or iViewNG based remote eye tracking devices
	REDm = 2,

	//! iView X based hi speed eye tracking devices
	HiSpeed = 3,

	//! iView X based MRI eye tracking devices
	MRI = 4,

	//! iView X based head mounted eye tracking devices
	HED = 5,

	//! iView X based custom devices like the mouse grabber
	Custom = 7,

	//! iViewRED based REDn eye tracking devices
	REDn = 8
};

/**
 * @enum ETApplication
 *
 * @brief ETApplication can be used to start iView X, iView X OEM or iViewNGServer
 * (iView eye tracking server) application dependent to the used eye tracking
 * device. Set this as a parameter in @ref iV_Start function.
 */
enum ETApplication {
	//! for iView X based devices like RED, HiSpeed, MRI, HED
	iViewX = 0,

	//! for RED-OEM based devices like RED-m or other customized RED-OEM devices
	iViewXOEM = 1,

	//! for RED250mobile and REDn devices
	iViewNG = 2
};

/**
 * @enum FilterType
 *
 * @brief FilterType can be used to select the filter that is used
 * with @ref iV_ConfigureFilter
 */
enum FilterType {
	//! Left and right gaze data channels are averaged.
	//! Both @ref EyeDataStruct in a @ref SampleStruct, e.g. obtained by @ref iV_GetSample, 
	//! contain equal gaze position data if the filter is anabled.
	//! Also data within recorded files is affected.
	//! The type of the parameter data from @ref iV_ConfigureFilter has to be converted to int*.
	//! The value of data can be [0;1] where 0 means averaging is disabled and 1 means averaging is enabled.
	Average = 0
};

/**
 * @enum FilterAction
 *
 * @brief FilterType can be used to select the action that is performed
 * when calling @ref iV_ConfigureFilter.
 */
enum FilterAction {
	//! query the current filter status
	Query = 0,

	//! configure filter parameters
	Set = 1
};

/**
 * @enum CalibrationPointUsageStatusEnum
 *
 * @brief This enum provides information about whether a calibration point was
 * actually used for the calibration or the reason why it was not used.
 *
 * Use @ref iV_GetCalibrationQuality to retrieve the calibration quality data
 */
enum CalibrationPointUsageStatusEnum {

	//! the calibration point was accepted and is used for the calibration
	calibrationPointUsed = 0,

	//! the calibration point was rejected and is not used for the calibration
	calibrationPointUnused = 1,

	//! the calibration point was rejected because no fixation was detected within reasonable time
	calibrationPointUnusedBecauseOfTimeout = 2,

	//! the calibration point was rejected because the detected fixation was to imprecise
	calibrationPointUnusedBecauseOfBadQuality = 3,

	//! no fixation was detected for the calibration point, but it is also not required
	calibrationPointIgnored = 4,
};

/**
 * @enum CalibrationStatusEnum
 *
 * @brief This enum provides information about the iView eye tracking server calibration status.
 *
 * If the device is not calibrated the iView eye tracking server will not deliver valid gaze data. Use the functions
 * @ref iV_GetCalibrationStatus to retrieve the calibration status and
 * @ref iV_Calibrate to perform a calibration.
 */
enum CalibrationStatusEnum {
	//! calibration status is unknown (i.e. if the connection is not established)
	calibrationUnknown = 0,

	//! the device is not calibrated and will not deliver valid gaze data
	calibrationInvalid = 1,

	//! the device is calibrated and will deliver valid gaze data
	calibrationValid = 2,

	//! the device is currently performing a calibration
	calibrationInProgress = 3
};

/**
* @enum RecordingState
*
* @brief Defines the recording states reported by the eye tracking server
*/
enum RecordingState {
	//! Recording state is idle. A recording was not started, no data has been collected.
	RecordingIdle = 0,

	//! Recording in progress. A recording was started and data is currently collected.
	RecordingRunning = 1,

	//! Recording is stopped. A recording is active, but currently stopped and no data is currently collected. 
	//! In this state the recording can either be finished or continued.
	RecordingStopped = 2,
};

/**
 * @enum REDGeometryEnum
 *
 * @brief Is used to select the valid data fields of @ref REDGeometryStruct
 */
enum REDGeometryEnum {
	//! use monitor integrated mode
	monitorIntegrated = 0,

	//! use standalone mode
	standalone = 1
};

/**
 * @enum TrackingMode
 *
 * @brief will be used for set and query of the tracking mode using @ref iV_SetTrackingMode
 * and @ref iV_GetTrackingMode.
 * See @ref TrackingMode,the @ref trackerParameter subsection and 
 * the SMI iView eye tracking server manual for further explanations.
 */
enum TrackingMode {
	//! SmartBinocular mode
	SmartBinocular,

	//! Monocular mode using only the left eye
	MonocularLeft,

	//! Monocular mode using only the right eye
	MonocularRight,

	//! Binocular mode
	Binocular,

	//! SmartMonocular mode
	SmartMonocular,
};

/** @}*/

/** @addtogroup structs Data Structures
 *  @{
 */

/**
 * @struct SystemInfoStruct
 *
 * @brief This struct provides information about the iView eye tracking server version
 * and the API version in use. To update data in @ref SystemInfoStruct use the function
 * @ref iV_GetSystemInfo.
 */
struct SystemInfoStruct {
		//! sample rate of eye tracking device in use
		int samplerate;

		//! major version number of iView eye tracking server in use
		int iV_MajorVersion;

		//! minor version number of iView eye tracking server in use
		int iV_MinorVersion;

		//! build number of iView eye tracking server in use
		int iV_Buildnumber;

		//! major version number of iView X SDK in use
		int API_MajorVersion;

		//! minor version number of iView X SDK in use
		int API_MinorVersion;

		//! build number of iView X SDK in use
		int API_Buildnumber;

		//! type of eye tracking device
		enum ETDevice iV_ETDevice;
};

/**
 * @struct SpeedModeStruct
 *
 * @brief This struct provides information about the speed modes used and supported by the connected iView eye tracking server.
 * They determine the sampling frequency (in Hz) of the eye tracker.
 * To update information in @ref SpeedModeStruct use function @ref iV_GetSpeedModes.
 */
struct SpeedModeStruct {
		//! version of the current data structure
		int version;

		//! the current sampling frequency
		int speedMode;

		//! number of supported speed modes
		int numberOfSpeedModes;

		//! an array of sampling frequencies supported by the connected iView eye tracking server;
		int speedModes[16];
};

/**
 * @struct CalibrationPointStruct
 *
 * @brief This struct provides information about the position of calibration points.
 * 
 * To update information in @ref CalibrationPointStruct during a calibration or validation
 * use function @ref iV_GetCurrentCalibrationPoint. Before or after the calibration use
 * @ref iV_GetCalibrationPoint.
 */
struct CalibrationPointStruct {
		//! number of calibration point
		//! the first calibration point has the number 1, the last one has the number given by @ref CalibrationStruct::method (see @ref calMethod).
		int number;

		//! horizontal position of calibration point [pixel]
		int positionX;

		//! vertical position of calibration point [pixel]
		int positionY;
};

/**
 * @struct CalibrationPointQualityStruct
 *
 * @brief This struct provides information about the fixation quality when a
 * calibration point was shown. If the request calibration quality
 * data is not available, the number and positionX/positionY will be
 * set to -1. User have to check these fields to make sure the
 * returned data is valid.
 *
 * Use @ref iV_GetCalibrationQuality to retrieve the calibration quality data
 */
struct CalibrationPointQualityStruct {
		//! number of calibration point
		//! the first calibration point is indexed with 1, the last one has the number given by @ref CalibrationStruct::method (see @ref calMethod).
		int number;

		//! horizontal position of calibration point [pixel]
		int positionX;

		//! vertical position of calibration point [pixel]
		int positionY;

		//! horizontal position of corrected fixation point [pixel]
		double correctedPorX;

		//! vertical position of corrected fixation point [pixel]
		double correctedPorY;

		//! horizontal standard deviation of the gaze samples, which represents
		//! the noise level of the fixation, given in [pixel]
		double standardDeviationX;

		//! vertical standard deviation of the gaze samples, which represents
		//! the noise level of the fixation, given in [pixel]
		double standardDeviationY;

		//! a flag indicating whether the calibration point was really used for
		//! the calibration or the reason why it was not used.
		enum CalibrationPointUsageStatusEnum usageStatus;

		//! quality index indicates how likely the user was really fixating on
		//! the calibration point when it was shown. It has a value between 0
		//! and 1. The higher the value, the more likely the user was
		//! fixating the calibration point as required.
		double qualityIndex;
};

/**
 * @struct EyeDataStruct
 *
 * @brief This struct provides numerical information about eye data.
 * @ref EyeDataStruct is part of @ref SampleStruct. To update information
 * in @ref SampleStruct use function @ref iV_GetSample or set the sample
 * callback with @ref iV_SetSampleCallback.
 *
 *  @image html "EyeDataStruct.png" "Gaze Position and Eye Position" 
 *  @image latex "EyeDataStruct.png" "Gaze Position and Eye Position" width=15cm
 * 
 *
 */
struct EyeDataStruct {
		//! horizontal gaze position on screen [pixel]
		double gazeX;

		//! vertical gaze position on screen [pixel]
		double gazeY;

		//! pupil diameter [mm]
		double diam;

		//! horizontal eye position relative to camera [mm]
		double eyePositionX;

		//! vertical eye position relative to camera [mm]
		double eyePositionY;

		//! distance to camera [mm]
		double eyePositionZ;
};

/**
 * @struct SampleStruct
 *
 * @brief This struct provides information about an eye data sample.
 * To update information in @ref SampleStruct use the function
 * @ref iV_GetSample or set the sample callback with @ref iV_SetSampleCallback.
 */
struct SampleStruct {
		//! timestamp of current gaze data sample [microseconds]
		long long timestamp;

		//! stores information of the left eye (see @ref EyeDataStruct for more information)
		struct EyeDataStruct leftEye;

		//! stores information of the right eye (see @ref EyeDataStruct for more information)
		struct EyeDataStruct rightEye;

		//! plane number of gaze data sample (only for HED HT)
		int planeNumber;
};

/**
 * @struct SampleStruct32
 *
 * @brief This struct provides information about a eye data samples.
 * To update information in @ref SampleStruct32 use the function
 * @ref iV_GetSample32. The difference to @ref SampleStruct is that
 * the timestamp will be stored in milliseconds instead of microseconds.
 */
struct SampleStruct32 {
		//! timestamp of current gaze data sample [milliseconds]
		double timestamp;

		//! stores information of the left eye (see @ref EyeDataStruct for more information)
		struct EyeDataStruct leftEye;

		//! stores information of the right eye (see @ref EyeDataStruct for more information)
		struct EyeDataStruct rightEye;

		//! plane number of gaze data sample
		int planeNumber;
};

/**
 * @struct EventStruct
 *
 * @brief This struct provides information about the last eye event that has
 * been calculated.
 * To update information in @ref EventStruct use function @ref iV_GetEvent or set
 * the event callback with with @ref iV_SetEventCallback.
 *
 * Note, that fixation events are detected using an online dispersion based algorithm. 
 * Online event detection can be used in gaze contingent application such as human-computer
 * interfaces. However, it is not intended for offline data analysis.
 */
struct EventStruct {
		//! type of eye event, 'F' for fixation (only fixations are supported)
		char eventType;

		//! related eye, 'l' for left eye, 'r' for right eye
		char eye;

		//! start time of the event [microseconds]
		long long startTime;

		//! end time of the event [microseconds]
		long long endTime;

		//! duration of the event [microseconds]
		long long duration;

		//! horizontal position of the fixation event [pixel]
		double positionX;

		//! vertical position of the fixation event [pixel]
		double positionY;
};

/**
 * @struct EventStruct32
 *
 * @brief This struct provides information about the last eye event that has
 * been calculated.
 * The difference to @ref EventStruct is that the timestamp will be stored in
 * milliseconds instead of microseconds and the order of the components are different.
 * To update information in @ref EventStruct32 use function @ref iV_GetEvent32.
 */
struct EventStruct32 {
		//! start time of the event [milliseconds]
		double startTime;

		//! end time of the event [milliseconds]
		double endTime;

		//! duration of the event [milliseconds]
		double duration;

		//! horizontal position of the fixation event [pixel]
		double positionX;

		//! vertical position of the fixation event [pixel]
		double positionY;

		//! type of eye event, 'F' for fixation (only fixations are supported)
		char eventType;

		//! related eye, 'l' for left eye, 'r' for right eye
		char eye;
};

/**
 * @struct EyePositionStruct
 *
 * @brief This value represents the relative position of the eye in the tracking box.
 * The 0 is defined at the center position. The value +1 defines the upper/right/far
 * maximum while the value -1 the lower/left/near maximum.
 * The position rating is related to the tracking monitor and represents how critical
 * the tracking and the position is, related to the border of the tracking box. The 0
 * is defined as the best eye position to be tracked while the value +1 defines that
 * the eye is almost not being tracked due to extreme upper/right/far position. The
 * value -1 defines that the eye is almost not being tracked due to extreme
 * lower/left/near position.
 * If the eye is not tracked at all the validity flag goes to 0 and all values for the
 * represented eye will be set to 0.
 */
struct EyePositionStruct {
		//! confidence of position and rating values [0; 1]
		int validity;

		//! horizontal position [-1; +1]
		double relativePositionX;

		//! vertical position [-1; +1]
		double relativePositionY;

		//! depth/distance position [-1; +1]
		double relativePositionZ;

		//! horizontal rating [-1; +1]
		double positionRatingX;

		//! vertical rating [-1; +1]
		double positionRatingY;

		//! distance rating [-1; +1]
		double positionRatingZ;
};

/**
 * @struct TrackingStatusStruct
 *
 * @brief This struct provides information about the relative eye ball position
 * within the tracking box. The information will be provided for each eye individually
 * as well as for the geometric center between both eyes. To update information in
 * @ref TrackingStatusStruct use the function @ref iV_GetTrackingStatus.
 */
struct TrackingStatusStruct {
		//! timestamp of current tracking status sample [microseconds]
		long long timestamp;

		//! stores information of the left eye (see EyePositionStruct for more information)
		struct EyePositionStruct leftEye;

		//! stores information of the right eye (see EyePositionStruct for more information)
		struct EyePositionStruct rightEye;

		//! stores information of the geometric center of both eyes (see EyePositionStruct for more information)
		struct EyePositionStruct total;
};

/**
 * @struct AccuracyStruct
 *
 * @brief This struct provides information about the last validation. The provided deviations are
 * the average absolute distances between meassured and expected gaze positions over all 
 * validation points.
 * A validation must have been successfully completed before the AccuracyStruct can be
 * updated. To update information in @ref AccuracyStruct use function @ref iV_GetAccuracy.
 */
struct AccuracyStruct {
		//! horizontal calculated deviation for left eye [degree]
		double deviationLX;

		//! vertical calculated deviation for left eye [degree]
		double deviationLY;

		//! horizontal calculated deviation for right eye [degree]
		double deviationRX;

		//! vertical calculated deviation for right eye [degree]
		double deviationRY;
};

/**
 * @struct GazeChannelQualityStruct
 *
 * @brief This struct provides information about the last validation.
 * A validation must have been successfully completed before the
 * GazeChannelQualityStruct can be updated. To update information in @ref
 * GazeChannelQualityStruct use function @ref iV_GetGazeChannelQuality.
 */
struct GazeChannelQualityStruct {
		//! Quality index of the left gaze channel. It lies in the range [0,1].
		//! A value > 0.5 means it is acceptable.
		//! A NaN value means it is not provided.
		double gazeChannelQualityLeft;

		//! Quality index of the right gaze channel. It lies in the range [0,1].
		//! A value > 0.5 means it is acceptable.
		//! A NaN value means it is not provided.
		double gazeChannelQualityRight;

		//! Quality index of the averaged gaze channel. It lies in the range [0,1].
		//! A value > 0.5 means it is acceptable.
		//! A NaN value means it is not provided.
		double gazeChannelQualityBinocular;
};

/**
 * @struct CalibrationStruct
 *
 * @brief Use this struct to customize the calibration and validation behavior.
 * To set calibration parameters with @ref CalibrationStruct use function
 * @ref iV_SetupCalibration before a calibration or validation is started.
 */
struct CalibrationStruct {
		//! select calibration method (default: 5)
		//! a bit mask is used to specify a new calibration workflow
		//! (see @ref calMethod)
		unsigned int method;

		//! draw calibration/validation by API (default: 1)
		int visualization;

		//! set display device [0: primary device (default), 1: secondary device]
		int displayDevice;

		//! set calibration/validation speed [0: slow (default), 1: fast]
		int speed;

		//! set calibration/validation point acceptance [2: full-automatic, 1: semi-automatic (default), 0: manual]
		int autoAccept;

		//! set calibration/validation target brightness [0..255] (default: 250)
		int foregroundBrightness;

		//! set calibration/validation background brightness [0..255] (default: 220)
		int backgroundBrightness;

		//! set calibration/validation target shape [IMAGE = 0, CIRCLE1 = 1, CIRCLE2 = 2 (default), CROSS = 3]
		int targetShape;

		//! set calibration/validation target size in pixel (minimum: 10 pixels, default: 20 pixels)
		int targetSize;

		//! select custom calibration/validation target (only if targetShape = 0)
		char targetFilename[256];
};

/**
 * @struct REDGeometryStruct
 *
 * @brief Use this struct to customize the RED geometry.
 * See chapter @ref redgeometry in the iView X SDK Manual for details.
 * For setting up the RED geometry parameters with
 * @ref REDGeometryStruct use function @ref iV_SetREDGeometry.
 */
struct REDGeometryStruct {

		//! defines which parameter is used.
		enum REDGeometryEnum redGeometry;

		//! monitor size [inch]
		//! can be set to 19 or 22
		//! used if @ref redGeometry is set to @ref monitorIntegrated only
		int monitorSize;

		//! name of the profile
		//! used if @ref redGeometry is set to @ref standalone only
		char setupName[256];

		//! horizontal stimulus calibration size [mm]
		//! used if @ref redGeometry is set to @ref standalone only
		int stimX;

		//! vertical stimulus calibration size [mm]
		//! used if @ref redGeometry is set to @ref standalone only
		int stimY;

		//! distance floor to stimulus screen [mm]
		//! used if @ref redGeometry is set to @ref standalone only
		int stimHeightOverFloor;

		//! distance floor to eye tracking device [mm]
		//! used if @ref redGeometry is set to @ref standalone only
		int redHeightOverFloor;

		//! distance eye tracking device to stimulus screen [mm]
		//! used if @ref redGeometry is set to @ref standalone only
		int redStimDist;

		//! eye tracking device inclination angle [degree]
		//! used if @ref redGeometry is set to @ref standalone only
		int redInclAngle;

		//! vertical distance eye tracking device to stimulus screen [mm]
		//! used if @ref redGeometry is set to @ref standalone only
		int redStimDistHeight;

		//! horizontal distance eye tracking device to stimulus screen [mm]
		//! used if @ref redGeometry is set to @ref standalone only
		int redStimDistDepth;
};

/**
 * @struct ImageStruct
 *
 * @brief Use this struct to get raw eye image, raw scene video image,
 * raw tracking monitor image or accuracy image: 
 * - For receiving raw eye image (format: monochrome 8bpp) use @ref iV_GetEyeImage, or 
 *   set the eye image callback with @ref iV_SetEyeImageCallback.
 * - For receiving raw scene video image (format: RGB 24bpp) use @ref iV_GetSceneVideo,
 *   or set the scene video callback with @ref iV_SetSceneVideoCallback. 
 * - For receiving raw tracking monitor image (format: BGR 24bpp) use @ref iV_GetTrackingMonitor, or
 *   set the tracking monitor callback with @ref iV_SetTrackingMonitorCallback.
 * - For receiving the accuracy image (format: BGR 24bpp) use @ref iV_GetAccuracyImage.
 * - For receiving the calibration quality image (format: BGR 24bpp) use @ref iV_GetCalibrationQualityImage
 */
struct ImageStruct {
		//! vertical size of the image [pixel]
		int imageHeight;

		//! horizontal size of the image [pixel]
		int imageWidth;

		//! image data size [byte]
		int imageSize;

		//! pointer to image data
		char * imageBuffer;
};

/**
 * @struct DateStruct
 *
 * @brief Use this struct to get the license due date of the device. Use the
 * function @ref iV_GetLicenseDueDate to update information in @ref DateStruct.
 */
struct DateStruct {
		//! day of license expiration
		int day;

		//! month of license expiration
		int month;

		//! year of license expiration
		int year;
};

/**
 * @struct AOIRectangleStruct
 *
 * @brief Use this struct to customize the AOI position on screen. @ref AOIRectangleStruct
 * is a part of @ref AOIStruct and can be defined with @ref iV_DefineAOI.
 */
struct AOIRectangleStruct {
		//! x-coordinate of left border of the AOI [pixel]
		int x1;

		//! x-coordinate of right border of the AOI [pixel]
		int x2;

		//! y-coordinate of upper border of the AOI [pixel]
		int y1;

		//! y-coordinate of lower border of the AOI [pixel]
		int y2;
};

/**
 * @struct AOIStruct
 *
 * @brief Use this struct to customize trigger AOIs. To define AOIs on screen, trigger
 * parameter and trigger values use @ref iV_DefineAOIPort and @ref iV_DefineAOI functions.
 */
struct AOIStruct {
		//! enable/disable trigger functionality [1: enabled, 0: disabled]
		int enabled;

		//! name of AOI
		char aoiName[256];

		//! group name of AOI
		char aoiGroup[256];

		//! position of AOI
		struct AOIRectangleStruct position;

		//! uses fixations or raw data as trigger [1: fixation hit, 0: raw data hit]
		int fixationHit;

		//! TTL output value
		int outputValue;

		//! message in idf data stream
		char outputMessage[256];

		//! ['l', 'r']
		char eye;
};

/** @}*/

/** @addtogroup typedefs Callback Function Types
 *  Note, that return values of callback functions are ignored by the iViewXAPI.
 *  @{
 */

/**
 * @brief calibration callback function type
 */
typedef int (CALLBACK * pDLLSetCalibrationPoint) (struct CalibrationPointStruct calibrationPoint);

/**
 * @brief AOI hit callback function type
 */
typedef int (CALLBACK * pDLLSetAOIHit) (int digitalOutoutValue);

/**
 * @brief sample callback function type
 */
typedef int (CALLBACK * pDLLSetSample) (struct SampleStruct rawDataSample);

/**
 * @brief event callback function type
 */
typedef int (CALLBACK * pDLLSetEvent) (struct EventStruct eventDataSample);

/**
 * @brief eye image callback function type
 */
typedef int (CALLBACK * pDLLSetEyeImage) (struct ImageStruct eyeImage);

/**
 * @brief scene image callback function type
 */
typedef int (CALLBACK * pDLLSetSceneVideo) (struct ImageStruct sceneVideo);

/**
 * @brief tracking monitor image callback function type
 */
typedef int (CALLBACK * pDLLSetTrackingMonitor) (struct ImageStruct trackingMonitor);

/** @}*/

/** @addtogroup functions Functions
 *  @{
 */

/**
 * @brief Aborts a calibration or validation if one is in progress. If the calibration or
 * validation function is visualizing the calibration area the @ref iV_Calibrate or
 * @ref iV_Validate function will return with RET_CALIBRATION_ABORTED.
 * @sa @listcalibration
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	RET_NO_VALID_DATA			no data available
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this
 *										function is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_AbortCalibration ();

/**
 * @brief Abort waiting for fixation for a calibration or validation point when the calibration or
 * validation is in progress. If the latest calibration point has been accepted by the iView eye tracking server,
 * the acceptance will be undone and the point unused. This allows the clients to customize
 * the controlling logic of the calibration workflow, esp. when the calibration UI is
 * implemented by user.
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this
 *										function is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_AbortCalibrationPoint ();

/**
 * @brief Accepts a calibration or validation point if the calibration or validation is in progress.
 * The participant needs to be tracked and has to fixate the calibration or validation point.
 * @sa @listcalibration
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	RET_NO_VALID_DATA			no data available
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this
 *										function is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_AcceptCalibrationPoint ();

/**
 * @brief Starts a calibration procedure. To proceed, the participant needs to be tracked and
 * has to fixate the calibration point. Depending on the calibration settings (which can
 * be changed using @ref iV_SetupCalibration and @ref iV_SetUseCalibrationKeys) the user can accept the calibration points
 * manually (by pressing [SPACE] or calling @ref iV_AcceptCalibrationPoint) or abort the
 * calibration (by pressing [ESC] or calling @ref iV_AbortCalibration)
 *
 * If the calibration is visualized by the API (@ref CalibrationStruct::visualization is set to 1)
 * the function will not return until the calibration has been finished (closed automatically)
 * or aborted (using [ESC]).
 *
 * If the @ref CalibrationStruct::visualization is set to 0, the function call returns
 * immediately. The user has to implement the visualization of calibration points. Information
 * about the current calibration point can be retrieved with @ref iV_GetCurrentCalibrationPoint
 * or with setting up the calibration callback using @ref iV_SetCalibrationCallback.
 * @sa @listcalibration
 *
 * @retval	RET_SUCCESS						intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED				no connection established
 * @retval	ERR_WRONG_DEVICE				eye tracking device required for this function
 *											is not connected
 * @retval	ERR_WRONG_CALIBRATION_METHOD	eye tracking device required for this
 *											calibration method is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Calibrate ();

/**
 * @brief Changes the position of a calibration point. This has to be done before the calibration
 * process is started. The parameter number refers to the calibration method used.
 * The change is applied to the currently selected profile.
 * @sa @listcalibration
 *
 * @param	number						selected calibration point
 * @param	positionX					new X position on screen [pixel]
 * @param	positionY					new Y position on screen [pixel]
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_NO_RESPONSE_FROM_IVIEWX	no response from iView X; check
 *										calibration name / identifier
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ChangeCalibrationPoint (int number, int positionX, int positionY);

/**
 * @brief Removes all trigger AOIs.
 * @sa @listaoi
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_AOI_ACCESS			failed to access AOI data
 * @retval	ERR_DEPRECATED_FUNCTION	function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ClearAOI ();

/**
 * @brief Clears the recorded data buffer. The recording buffer needs to be stopped using "iV_StopRecording"
 * before it can be cleared.
 * If you are using an *HED* device, the scene video buffer is cleared, too.
 * @sa @listeyedatarecording
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function
 *										is not connected
 * @retval	ERR_EMPTY_DATA_BUFFER		recording buffer is empty
 * @retval	ERR_RECORDING_DATA_BUFFER	recording is activated
 * @retval	ERR_PAUSED_DATA_BUFFER		recording is in pause state
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 *
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ClearRecordingBuffer ();

/**
 * @brief Queries or sets filter parameters.
 * The usage of the parameter data depends on the parameter action,
 *
 * @param	filter						filter type that is configured. See @ref FilterType
 * @param	action						type of action. See @ref FilterAction
 * @param	data						A void pointer that must be casted to the data type defined by the filter type.
 *										Please refer to @ref FilterType for details.
 *										Content of the parameter depends on filter action, see @ref FilterType
 *										- @ref FilterAction::Query : data is filled with current filter settings
 *										- @ref FilterAction::Set : data is passed to configure the filter
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_PARAMETER			parameter out of range
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ConfigureFilter (enum FilterType filter, enum FilterAction action, void * data);

/**
 * @brief Establishes a connection to the iView eye tracking server. @ref iV_Connect will
 * not return until a connection has been established. If no connection can be
 * established, the function will return after the time span defined by
 * @ref iV_SetConnectionTimeout. Default time span is 3 seconds.
 *
 * @attention For systems running with SMI iViewRED 4.2 or higher, it is no longer required to
 * specify @p recvIPAddress and @p receivePort. The passed value will be ignored.
 * @sa @listsystemstartstop
 *
 * @param	sendIPAddress							IP address of the eye tracking servercomputer
 * @param	sendPort								port being used by iView X SDK for sending data to the remote eye tracking server
 * @param	recvIPAddress							IP address of local computer
 * @param	receivePort								port being used by iView X SDK for receiving data from the remote eye tracking server
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_SERVER_NOT_FOUND					no iView eye tracking server detected
 * @retval	ERR_EYETRACKING_APPLICATION_NOT_RUNNING	no eye tracking application running
 * @retval	ERR_WRONG_PARAMETER						connection parameter out of range
 * @retval	ERR_WRONG_COMMUNICATION_PARAMETER		invalid license set (only applys for SMI iViewRED 4.2 or higher)
 * @retval	ERR_COULD_NOT_CONNECT					failed to establish connection
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Connect (char * sendIPAddress, int sendPort, char * recvIPAddress, int receivePort);

/**
 * @brief Establishes a connection to the iView eye tracking server. @ref iV_ConnectLocal will
 * not return until a connection has been established. If no connection can be
 * established the function will return after the time span defined by
 * @ref iV_SetConnectionTimeout. Default time span is 3 seconds.
 *
 * iV_ConnectLocal can only connect with an eye tracking server running on
 * the same PC.
 * @sa @listsystemstartstop
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_SERVER_NOT_FOUND					no iView eye tracking server detected
 * @retval	ERR_EYETRACKING_APPLICATION_NOT_RUNNING	no eye tracking application running
 * @retval	ERR_WRONG_COMMUNICATION_PARAMETER		invalid license set (only applys for SMI iViewRED 4.2 or higher)
 * @retval	ERR_COULD_NOT_CONNECT					failed to establish connection
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ConnectLocal ();

/**
 * @brief Wakes up and enables the eye tracking application from suspend mode to
 * continue processing gaze data. The application can be set to suspend mode
 * by calling @ref iV_PauseEyetracking
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ContinueEyetracking ();

/**
 * @brief Continues gaze data recording. iV_ContinueRecording does not return until gaze recording is continued.
 * Before it can be continued, the data needs to be paused using.
 * @ref iV_PauseRecording. Additionally this function allows a message to be stored inside the
 * idf data buffer.
 * @attention An *HED* video recording can neither be paused nor continued.
 * @sa @listeyedatarecording
 *
 * @param	etMessage					text message that will be written to data file
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function
 *										is not connected
 * @retval	ERR_EMPTY_DATA_BUFFER		recording buffer is empty
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ContinueRecording (char * etMessage);

/**
 * @brief Defines an AOI. The API can handle up to 20 AOIs.
 * @sa @listaoi
 *
 * @param	aoiData					see reference information for @ref AOIStruct
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER		parameter out of range
 * @retval	ERR_DEPRECATED_FUNCTION	function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_DefineAOI (struct AOIStruct * aoiData);

/**
 * @brief Selects a port for sending out TTL trigger.
 * @sa @listaoi
 *
 * @param	port					port address
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER		parameter out of range
 * @retval	ERR_COULD_NOT_OPEN_PORT	failed to open port
 * @retval	ERR_DEPRECATED_FUNCTION	function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_DefineAOIPort (int port);

/**
 * @brief Deletes the geometry setup with the given name. It is not possible
 * to delete a geometry profile if it is currently in use. See chapter
 * @ref redgeometry in the iView X SDK Manual.
 *
 * @param	setupName					name of the geometry setup which will be deleted
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_PARAMETER			parameter out of range
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function
 *										is not connected
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_DeleteREDGeometry (char * setupName);

/**
 * @brief Disables all AOIs with the given name.
 * @sa @listaoi
 *
 * @param	aoiName						name of the AOI which will be disabled
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA			no data available
 * @retval	ERR_AOI_ACCESS				failed to access AOI data 
 * @retval	ERR_DEPRECATED_FUNCTION		function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_DisableAOI (char * aoiName);

/**
 * @brief Disables an AOI group.
 * @sa @listaoi
 *
 * @param	aoiGroup				name of the AOI group which will be disabled
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA		no data available
 * @retval	ERR_AOI_ACCESS			failed to access AOI data
 * @retval	ERR_DEPRECATED_FUNCTION	function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_DisableAOIGroup (char * aoiGroup);

/**
 * @brief Disables the raw data filter. The gaze data filter can be enabled
 * using @ref iV_EnableGazeDataFilter.
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_DisableGazeDataFilter ();

/**
 * @brief Disables a CPU high performance mode allowing the CPU to reduce the performance.
 * @sa @ref iV_EnableProcessorHighPerformanceMode
 *
 * @retval	RET_SUCCESS			intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED	no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_DisableProcessorHighPerformanceMode ();

/**
 * @brief Disconnects from iView eye tracking server. @ref iV_Disconnect will not
 * return until the connection has been disconnected. After this function has
 * been called no other function or device can communicate with
 * iView eye tracking server.
 * @sa @listsystemstartstop
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_DELETE_SOCKET		failed to delete sockets
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Disconnect ();

/**
 * @brief Enables all AOIs with the given name.
 * @sa @listaoi
 *
 * @param	aoiName					name of the AOI which will be enabled
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA		no data available
 * @retval	ERR_AOI_ACCESS			failed to access AOI data
 * @retval	ERR_DEPRECATED_FUNCTION	function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_EnableAOI (char * aoiName);

/**
 * @brief Enables an AOI group.
 * @sa @listaoi
 *
 * @param	aoiGroup				name of the AOI group which will be enabled
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA		no data available
 * @retval	ERR_AOI_ACCESS			failed to access AOI data
 * @retval	ERR_DEPRECATED_FUNCTION	function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_EnableAOIGroup (char * aoiGroup);

/**
 * @brief Enables a gaze data filter. This API bilateral filter was implemented due
 * to special human-computer interaction (HCI) application requirements. It smoothes 
 * gaze position data in @ref EyeDataStruct::gazeX and @ref EyeDataStruct::gazeY contained
 * in @ref SampleStruct, e.g. obtained by @ref iV_GetSample.
 * The gaze data filter can be disabled using @ref iV_DisableGazeDataFilter.
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_EnableGazeDataFilter ();

/**
 * @brief Enables a CPU high performance mode to prevent the CPU from reducing the 
 * eye tracking performance, e.g. when entering an energy saving state.

 * @sa @ref iV_DisableProcessorHighPerformanceMode
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_EnableProcessorHighPerformanceMode ();

/**
 * @brief Updates @p accuracyData with validated accuracy results. Before
 * accuracy data is accessible the accuracy needs to be validated with
 * @ref iV_Validate. If the parameter @p visualization is set to 1 the accuracy
 * data will be visualized in a dialog window.
 * @sa @listvalidation and the chapter @ref validation in the iView X SDK Manual.
 *
 * @param	accuracyData			see reference information for @ref AccuracyStruct
 * @param	visualization			values:
 *									- 0: no visualization
 *									- 1: accuracy data will be visualized in a separate window similar to @ref iV_ShowAccuracyMonitor
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA		no data available
 * @retval	ERR_NOT_CONNECTED		no connection established
 * @retval	ERR_NOT_CALIBRATED		system is not calibrated
 * @retval	ERR_NOT_VALIDATED		system is not validated
 * @retval	ERR_WRONG_PARAMETER		parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetAccuracy (struct AccuracyStruct * accuracyData, int visualization);

/**
 * @brief Updates @p imageData struct with drawn accuracy results (format: BGR 24bpp). Before
 * accuracy data is accessible the accuracy needs to be validated with
 * @ref iV_Validate.
 * The image depicts all validation points and the corresponding measured gaze point
 * for the left and right eye during fixation.
 *
 *  @image html "accuracy_monitor.png" "Accuracy Image" 
 *  @image latex "accuracy_monitor.png" "Accuracy Image" width=10cm
 *
 * @sa @listvalidation and the chapter @ref validation in the iView X SDK Manual.
 *
 * @param	imageData				see reference information for @ref ImageStruct
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 * @retval	ERR_NOT_CALIBRATED		system is not calibrated
 * @retval	ERR_NOT_VALIDATED		system is not validated
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetAccuracyImage (struct ImageStruct * imageData);

/**
 * @brief Returns the current AOI value.
 * 
 * @sa @listaoi
 *
 * @param	aoiOutputValue			provides the AOI output value
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 * @retval	ERR_AOI_NOT_DEFINED		no defined AOI found
 * @retval	ERR_DEPRECATED_FUNCTION	function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetAOIOutputValue (int * aoiOutputValue);

/**
 * @brief Retrieves the available LPT ports from the computer running the eye tracking server
 * which can be used for LPT data recording. The caller has to supply a sufficiently large
 * string buffer to the function. The buffer is filled with a string list containing the names
 * of the available LPT port, separated by a semi colon (';'). An empty string indicates that
 * there are no suitable LPT ports available.
 * @attention This fuction is available for SMI iViewRED 4.2 and later versions.
 *
 *
 * @param	buffer					The buffer that is filled with a list of available
 * 									LPT port names, separated by a semi colon (';')
 * @param	bufferSize				pointer to a variable that contains the size of the buffer
 * 									in bytes; when the function returns with RET_SUCCESS, the variable
 * 									will contain the size of the used buffer, including the terminating
 * 									'@\0' character
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_INSUFFICIENT_BUFFER_SIZE	the provided buffer is to small and has not been altered
 * @retval	ERR_WRONG_PARAMETER		either the buffer or bufferSize parameter are NULL
 * @retval	ERR_NOT_CONNECTED		no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetAvailableLptPorts (char * buffer, int * bufferSize);

/**
 * @brief Updates stored @p calibrationData information with currently selected parameters.
 * @sa @listcalibration
 *
 * @param	calibrationData			see reference information for @ref CalibrationStruct
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCalibrationParameter (struct CalibrationStruct * calibrationData);

/**
 * @brief Delivers information about a calibration point.
 * @sa @listcalibration
 *
 * @param	calibrationPointNumber	number of requested calibration point
 * @param	calibrationPoint		information of requested calibration point, stored in @ref CalibrationPointStruct
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCalibrationPoint (int calibrationPointNumber,
        struct CalibrationPointStruct * calibrationPoint);

/**
 * @brief Delivers fixation quality information about a calibration point. If
 * the passed parameter @p left or @p right is NULL, no data will be returned.
 *
 * @param	calibrationPointNumber	number of requested calibration point
 * @param	left					requested quality information for the left eye, stored in @ref CalibrationPointQualityStruct
 * @param	right					requested quality information for the right eye, stored in @ref CalibrationPointQualityStruct
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCalibrationQuality (int calibrationPointNumber,
        struct CalibrationPointQualityStruct * left, struct CalibrationPointQualityStruct * right);

/**
 * @brief Updates @p imageData struct with drawn calibration quality
 * data (format: BGR 24bpp). A calibration is required before calling this function.
 * 
 * The image visualizes accuracy data for the calibration points similar to the
 * accuracy image obtained by @ref iV_GetAccuracyImage.
 *
 * @param	imageData				see reference information for @ref ImageStruct
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 * @retval	ERR_NOT_CALIBRATED		system is not calibrated
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCalibrationQualityImage (struct ImageStruct * imageData);

/**
 * @brief Updates @p calibrationStatus information. The client needs to be connected
 * to the iView eye tracking server.
 * @sa @listcalibration
 *
 * @param	calibrationStatus						see reference information for @ref CalibrationStatusEnum
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no new data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCalibrationStatus (enum CalibrationStatusEnum * calibrationStatus);

/**
 * @brief Updates data in @p currentCalibrationPoint with the current calibration point position.
 * @sa @listcalibration
 *
 * @param	currentCalibrationPoint	information of requested calibration point, stored in @ref CalibrationPointStruct
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA		no new data available
 * @retval	ERR_NOT_CONNECTED		no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCurrentCalibrationPoint (struct CalibrationPointStruct * currentCalibrationPoint);

/**
 * @brief Gets the currently loaded eye tracker geometry. See chapter @p redgeometry in the iView X SDK Manual
 * and @listgeometry
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_WRONG_DEVICE						eye tracking device required for this function
 *													is not connected
 * @retval	ERR_FEATURE_NOT_LICENSED				feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCurrentREDGeometry (struct REDGeometryStruct * redGeometry);

/**
 * @brief Provides the current eye tracker timestamp in microseconds.
 * @sa @listdataacquisition
 *
 * @param	currentTimestamp			information of requested time stamp
 *
 * @retval RET_SUCCESS					intended functionality has been fulfilled
 * @retval RET_NO_VALID_DATA			no new data available
 * @retval ERR_NOT_CONNECTED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCurrentTimestamp (long long * currentTimestamp);

/**
 * @brief Queries the device name information of the connected device.
 *
 * @param	deviceName				the name of the requested device
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA		no data available
 * @retval	ERR_NOT_CONNECTED		no connection established
 * @retval	ERR_WRONG_DEVICE		eye tracking device required for this function
 *									is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetDeviceName (char deviceName[64]);

/**
 * @brief Updates data from @p eventDataSample with current event data.
 * @sa @listdataacquisition
 *
 * @param	eventDataSample				see reference information for @ref EventStruct
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA			no new data available
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetEvent (struct EventStruct * eventDataSample);

/**
 * @brief Updates data from @p eventDataSample with current event data.
 * @sa @listdataacquisition
 *
 * @param	eventDataSample				see reference information for @ref EventStruct32
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA			no new data available
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetEvent32 (struct EventStruct32 * eventDataSample);

/**
 * @brief Updates @p imageData with current eye image (format: monochrome 8bpp).
 *
 *  @image html "eye_image.png" "Eye Image" 
 *  @image latex "eye_image.png" "Eye Image" width=10cm
 *
 * @param	imageData					see reference information for @ref ImageStruct
 *
 * @retval RET_SUCCESS					intended functionality has been fulfilled
 * @retval RET_NO_VALID_DATA			no new data available
 * @retval ERR_NOT_CONNECTED			no connection established
 * @retval ERR_WRONG_DEVICE				eye tracking device required for this function
 *										is not connected
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetEyeImage (struct ImageStruct * imageData);

/**
 * @brief Gets the device specific feature key. Used for RED-OEM, RED250mobile and REDn devices only.
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function
 *										is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetFeatureKey (long long * featureKey);

/**
 * @brief Retrieve gaze quality data.
 * Fills @p qualityData with validated accuracy results. Before
 * quality data is accessible the system needs to be validated with @ref iV_Validate.
 * @sa @listvalidation and the chapter @ref validation in the iView X SDK Manual.
 *
 * @param	qualityData				see reference information for @ref GazeChannelQualityStruct
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA		no data available
 * @retval	ERR_NOT_CONNECTED		no connection established
 * @retval	ERR_NOT_CALIBRATED		system is not calibrated
 * @retval	ERR_NOT_VALIDATED		system is not validated
 * @retval	ERR_WRONG_PARAMETER		parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetGazeChannelQuality (struct GazeChannelQualityStruct * qualityData);

/**
 * @brief Gets all available profiles by name. They will be written comma-separated
 * in the char buffer. The user needs to ensure that the buffer is sufficiently large to
 * hold the list of available profiles.
 * @sa @ref redgeometry in the iView X SDK Manual and @listgeometry
 *
 * @param	maxSize					the length of the string profileNames
 * @param	profileNames			an empty string where profile names will be put in
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 * @retval	ERR_WRONG_PARAMETER		parameter out of range
 * @retval	ERR_WRONG_DEVICE		eye tracking device required for this
 *									function is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetGeometryProfiles (int maxSize, char * profileNames);

/**
 * @brief Gets the system license expiration date. The license will not expire if the
 * license is set to 00.00.0000.
 * @sa @listsystemstartstop
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetLicenseDueDate (struct DateStruct * licenseDueDate);

/**
* @brief Queries the recording state of the eye tracking server. This function can be used to check
* if the eye tracking server is currently performing a recording.
* @sa @listeyedatarecording @ref RecordingState
*
* @param	recordingState				the variable is updated to reflect the current recording state
*
* @retval	RET_SUCCESS					intended functionality has been fulfilled
* @retval	ERR_WRONG_PARAMETER			@p recordingState is NULL
* @retval	ERR_NOT_CONNECTED			no connection established
* @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
**/
DLL_DEC_PRE int DLL_DEC_POST iV_GetRecordingState(enum RecordingState * recordingState);

/**
 * @brief Gets the geometry data of a requested profile without selecting them.
 * See chapter @ref redgeometry
 * in the iView X SDK Manual and @listgeometry
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_PARAMETER			parameter out of range
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this
 *										function is not connected
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetREDGeometry (char * profileName, struct REDGeometryStruct * redGeometry);

/**
 * @brief Updates data in @p rawDataSample with current eye tracking data.
 * @sa @listdataacquisition
 *
 * @param	rawDataSample			see reference information for @ref SampleStruct
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA		no new data available
 * @retval	ERR_NOT_CONNECTED		no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetSample (struct SampleStruct * rawDataSample);

/**
 * @brief Updates data in @p rawDataSample with current eye tracking data sample.
 * @sa @listdataacquisition
 *
 * @param	rawDataSample			see reference information for @ref SampleStruct32
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA		no new data available
 * @retval	ERR_NOT_CONNECTED		no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetSample32 (struct SampleStruct32 * rawDataSample);

/**
 * @brief Updates @p imageData with current scene video image  (format: RGB 24bpp).
 * @attention This functions is available for *HED* devices only.
 *
 * @param	imageData				see reference information for @ref ImageStruct
 *
 * @retval RET_SUCCESS				intended functionality has been fulfilled
 * @retval RET_NO_VALID_DATA		no new data available
 * @retval ERR_NOT_CONNECTED		no connection established
 * @retval ERR_WRONG_DEVICE			eye tracking device required for this function
 *									is not connected
 * @retval	ERR_DEPRECATED_FUNCTION	function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetSceneVideo (struct ImageStruct * imageData);

/**
 * @brief Retrieve the serial number information of the connected device.
 * @sa @listsystemstartstop
 *
 * @param	serialNumber				the serial number of the requested device
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA			no data available
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function
 *										is not connected
 * @retval	ERR_WRONG_IVIEWX_VERSION	wrong version of iView X
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetSerialNumber (char serialNumber[64]);

/**
 * @brief This function retrieves the speed modes used and supported by the connected iView eye tracking server.
 * @attention This fuction is available for SMI iViewRED 4.2 and later versions.
 * @sa @ref SpeedModeStruct for more details.
 *
 * @retval RET_SUCCESS			intended functionality has been fulfilled
 * @retval ERR_WRONG_PARAMTER	the requested speed mode is not supported by the connected device
 * @retval ERR_NOT_CONNECTED	no connection established
 * @retval ERR_WRONG_DEVICE		eye tracking device required for this function is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetSpeedModes (struct SpeedModeStruct * speedModes);

/**
 * @brief Query system information.
 * @p systemInfoData is updated with current system information.
 * @sa @listsystemstartstop
 *
 * @param	systemInfoData			see reference information for @ref SystemInfoStruct
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA		no data available
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetSystemInfo (struct SystemInfoStruct * systemInfoData);

/**
 * @brief Updates @p imageData with current tracking monitor image (format: BGR 24bpp).
 * 
 * The tracking monitor image depicts the positions of both eyes and shows notification arrows 
 * if the patricipant is not properly positioned infront of the eye tracker. 
 * The tracking monitor is useful to validate the positioning before and during a recording session.
 *
 *  @image html "tracking_monitor.png" "Tracking Monitor Image" 
 *  @image latex "tracking_monitor.png" "Tracking Monitor Image" width=10cm
 *
 * @param	imageData				see reference information for @ref ImageStruct
 *
 * @retval RET_SUCCESS				intended functionality has been fulfilled
 * @retval RET_NO_VALID_DATA		no new data available
 * @retval ERR_NOT_CONNECTED		no connection established
 * @retval ERR_WRONG_DEVICE			eye tracking device required for this function
 *									is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetTrackingMonitor (struct ImageStruct * imageData);

/**
* @brief Queries iView eye tracking server tracking mode. See @ref trackerParameter subsection and
* iView eye tracking server manual for further explanations.
* @attention This function is available with SMI iViewRED 4.4 and later versions
*
* @param	mode						pointer to a variable to hold the selected tracking mode
*
* @retval	RET_SUCCESS					intended functionality has been fulfilled
* @retval	ERR_NOT_CONNECTED			no connection established
* @retval	ERR_WRONG_PARAMETER			mode is NULL
**/
DLL_DEC_PRE int DLL_DEC_POST iV_GetTrackingMode(enum TrackingMode* mode);

/**
 * @brief Updates @p trackingStatus with current tracking status.
 * 
 * This function can be used to get the current eye positions.
 *
 * @param	trackingStatus			see reference information for @ref TrackingStatusStruct
 *
 * @retval RET_SUCCESS				intended functionality has been fulfilled
 * @retval RET_NO_VALID_DATA		no new data available
 * @retval ERR_NOT_CONNECTED		no connection established
 * @retval ERR_WRONG_DEVICE			eye tracking device required for this function
 *									is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetTrackingStatus (struct TrackingStatusStruct * trackingStatus);

/**
 * @brief Gets the currently set interaction key status for the calibration and validation process. 
 * If @p enableKeys is 0 all available user interaction keys:
 * - *SPACE* for accepting calibration/validation points
 * - *ESC* for aborting calibration/validation
 * - *TAB* for skipping a point (only SMI iViewRED 4.2 or later)
 *
 * are disabled.
 * @sa @listvalidation @listcalibration
 *
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetUseCalibrationKeys (int * enableKeys);

/**
 * @brief Hides accuracy monitor window which can be opened by @ref iV_ShowAccuracyMonitor.
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_WINDOW_IS_CLOSED		window is already closed
 * @retval	ERR_NOT_CONNECTED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_HideAccuracyMonitor ();

/**
 * @brief Hides eye image monitor window which can be opened by @ref iV_ShowEyeImageMonitor.
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_WINDOW_IS_CLOSED		window is already closed
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_HideEyeImageMonitor ();

/**
 * @brief Hides scene video monitor window which can be opened by @ref iV_ShowSceneVideoMonitor.
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_WINDOW_IS_CLOSED		window is already closed
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_DEPRECATED_FUNCTION		function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_HideSceneVideoMonitor ();

/**
 * @brief Hides tracking monitor window which can be opened by @ref iV_ShowTrackingMonitor.
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_WINDOW_IS_CLOSED		window is already closed
 * @retval	ERR_NOT_CONNECTED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_HideTrackingMonitor ();

/**
 * @brief Checks if connection to iView eye tracking server is still established.
 * @sa @listsystemstartstop
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_IsConnected ();

/**
 * @brief Loads a previously saved calibration. A calibration has to be saved by using
 * @ref iV_SaveCalibration.
 * @sa @listcalibration
 *
 * @param	name						calibration name or identifier
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_IVIEWX_VERSION	wrong version of iView X
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this
 *										function is not connected
 * @retval	ERR_NO_RESPONSE_FROM_IVIEWX	no response from iView X; check
 *										calibration name or identifier
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_LoadCalibration (char * name);

/**
 * @brief Writes @p logMessage into log file
 *
 * @param	logMessage				message that shall be written to the log file
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_ACCESS_TO_FILE		failed to access log file
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Log (char * logMessage);

/**
 * @brief Suspend the eye tracking application and disables calculation of gaze data.
 * The application can be reactivated by calling @ref iV_ContinueEyetracking.
 * @sa @listsystemstartstop
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_PauseEyetracking ();

/**
 * @brief Pauses gaze data recording.
 * @ref iV_PauseRecording does not return until gaze recording is paused.
  *@attention An HED video recording can neither be paused nor continued.
 * @sa @listeyedatarecording
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function
 *										is not connected
 * @retval	ERR_EMPTY_DATA_BUFFER		recording buffer is empty
 * @retval	ERR_FULL_DATA_BUFFER		data buffer is full
 * @retval	ERR_PAUSED_DATABUFFER		already in pause state
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_PauseRecording ();

/**
 * @brief Disconnects and closes iView eye tracking server. After this function
 * has been called no other function or application can communicate
 * with iView eye tracking server.
 * @sa @listsystemstartstop
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_DELETE_SOCKET			failed to delete sockets
 * @retval	ERR_WRONG_IVIEWX_VERSION	wrong version of iView X
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Quit ();

/**
 * @brief Restarts a calibration procedure with a point from the latest calibration process. The
 * point is specified by its index in the calibration point profile (counted from 1).
 * If the requested point is not found, an error code will be returned.
 * The number of calibration points can be retrieved via @ref iV_GetCalibrationQuality.
 *
 * This function can be used to improve the final calibration quality in cases when some points
 * from the previous calibration were missed unexpectedly. With this function you can
 * re-include that point to your calibration.
 *
 * This function follows the workflow of @ref iV_Calibrate except that @ref iV_SetupCalibration
 * must not be called after the end of the last calibration and before calling this function.
 *
 * If @ref CalibrationStruct::visualization was set to 0, @ref iV_RecalibrateOnePoint
 * returns immediately. The user has to care about the visualization of calibration points.
 * Information about the current calibration point can be retrieved with
 * @ref iV_GetCurrentCalibrationPoint or with setting up the calibration callback using
 * @ref iV_SetCalibrationCallback.
 * @sa @listcalibration
 *
 * @retval	RET_SUCCESS						intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED				no connection established
 * @retval	ERR_WRONG_PARAMETER				the point to recalibrate is not found
 * @retval	ERR_NOT_CALIBRATED				no previous calibration exists
 * @retval	ERR_WRONG_DEVICE 				eye tracking device required for this function
 *											is not connected
 * @retval	ERR_WRONG_CALIBRATION_METHOD	eye tracking device required for this
 *											calibration method is not connected or the method chosen for
 *											recalibration is different to the previous calibration.
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_RecalibrateOnePoint (int number);

/**
 * @brief Releases the port for sending TTL trigger.
 * @sa @listaoi
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_COULD_NOT_CLOSE_PORT	failed to close TTL port
 * @retval	ERR_DEPRECATED_FUNCTION		function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ReleaseAOIPort ();

/**
 * @brief Removes all AOIs with the given name.
 * @sa @listaoi
 *
 * @param	aoiName					name of the AOI which will be removed
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA		no data available
 * @retval	ERR_AOI_ACCESS			failed to access AOI data
 * @retval	ERR_DEPRECATED_FUNCTION	function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_RemoveAOI (char * aoiName);

/**
 * @brief Resets all calibration points to its default position.
 * @sa @listcalibration
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ResetCalibrationPoints ();

/**
 * @brief Saves a calibration with a custom name. To save a calibration
 * it is required that a successful calibration already has been completed.
 * @sa @listcalibration
 *
 * @param	name						calibration name / identifier
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_NOT_CALIBRATED			system is not calibrated
 * @retval	ERR_WRONG_IVIEWX_VERSION	wrong version of iView X
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this
 *										function is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SaveCalibration (char * name);

/**
 * @brief Writes recorded data buffer to disc. The data recording needs to be stopped using
 * @ref iV_StopRecording before the data buffer can be saved to given location.
 * The @p filename can include the path.
 * If the connected eye tracking device is an *HED*, scene video buffer is written, too.
 * @ref iV_SaveData will not return until the data has been saved.
 *
 * @param	filename					full path including the filename of the data file being created
 * @param	description					Optional experiment description tag stored in the idf file.
 *										This tag is available in BeGaze and in the text export from an idf file.
 * @param	user						Optional name of test person.
 *										This tag is available in BeGaze and in the text export from an idf file.
 * @param	overwrite					Overwriting policy.
 *										- 0: do not overwrite file @p filename if it already exists
 *										- 1: overwrite file @p filename if it already exists
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_PARAMETER			parameter out of range
 * @retval	ERR_EMPTY_DATA_BUFFER		recording buffer is empty
 * @retval	ERR_RECORDING_DATA_BUFFER	recording is in running, call  @ref iV_StopRecording recording before calling @ref iV_SaveData
 * @retval	ERR_PAUSED_DATA_BUFFER		recording is in pause state
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SaveData (char * filename, char * description, char * user, int overwrite);

/**
 * @brief Selects a predefined geometry profile. See chapter @ref redgeometry
 * in the iView X SDK Manual and @listgeometry
 *
 * @param	profileName					name of the selected profile which should be selected
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this
 *										function is not connected
 * @retval	ERR_FEATURE_NOT_LICENSED	this feature was not licensed by the API user
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SelectREDGeometry (char * profileName);

/**
 * @brief Sends a remote command to iView eye tracking server. Please refer to
 * the iView X help file for further information about remote commands.
 * Important Note: This function is temporary and will not be supported
 * in subsequent versions.
 * @sa @listeyedatarecording
 *
 * @param	etMessage				iView X remote command
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED		no connection established
 * @retval	ERR_WRONG_PARAMETER		parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SendCommand (char * etMessage);

/**
 * @brief Sends a text message to iView X idf recording data file. If the @p etMessage
 * has the suffix ".jpg", ".bmp", ".png", or ".avi" BeGaze will separate the
 * data buffer automatically into according trials.
 * @sa @listeyedatarecording
 *
 * @param	etMessage					Any text message to separate trials (image name containing
 *										extensions) or any idf data marker
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_EMPTY_DATA_BUFFER		no recording in progress
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SendImageMessage (char * etMessage);

/**
 * @brief Sets a callback function for the AOI hit functions. The function will be called
 * if the iView eye tracking server has calculated an AOI hit. For usage of this
 * function at least on AOI needs to be defined.
 * @attention Algorithms with high processor usage and long calculation time
 * should not run within this callback due to a higher probability of data loss
 * @sa @listaoi
 * @sa @listdataacquisition
 *
 * @param	pAOIHitCallbackFunction		pointer to AOIHitCallbackFunction
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER			parameter out of range
 * @retval	ERR_DEPRECATED_FUNCTION		function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetAOIHitCallback (pDLLSetAOIHit pAOIHitCallbackFunction);

/**
 * @brief Sets a callback function for the calibration and validation process. The
 * callback function will be called after a calibration or validation was started,
 * after a calibration or validation point was accepted, or if the calibration or
 * validation was finished successfully or unsuccessfully.
 * @sa @listcalibration
 *
 * @param	pCalibrationCallbackFunction	pointer to CalibrationCallbackFunction
 *
 * @retval	RET_SUCCESS						intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER				parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetCalibrationCallback (pDLLSetCalibrationPoint pCalibrationCallbackFunction);

/**
 * @brief Defines a customized timeout for how long @ref iV_Connect tries to connect
 * to iView eye tracking server.
 * @sa @listsystemstartstop
 *
 * @param	time					the time in seconds iV_Connect is waiting for an eye tracking server response
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER		parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetConnectionTimeout (int time);

/**
 * @brief Sets a callback function for the event data. The function will be called if a
 * real-time detected fixation has been started or ended.
 * @attention Algorithms with high processor usage and long calculation time
 * should not run within this callback due to a higher probability of data loss
 * @sa @listdataacquisition
 *
 * @param	pEventCallbackFunction		pointer to EventCallbackFunction
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER			parameter out of range
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetEventCallback (pDLLSetEvent pEventCallbackFunction);

/**
 * @brief Defines the detection parameter for online fixation detection algorithm.
 * @sa @listdataacquisition
 *
 * @param	minDuration				minimum fixation duration [ms]
 * @param	maxDispersion			maximum dispersion [pixel] for head tracking systems
 *									or [deg] for non head tracking systems
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER		parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetEventDetectionParameter (int minDuration, int maxDispersion);

/**
 * @brief Sets a callback function for the eye image data. The function will be called if a
 * new eye image is available. The image format is monochrome 8bpp.
 * @attention Algorithms with high processor usage and long calculation time
 * should not run within this callback due to a higher probability of data loss
 *
 * @param	pEyeImageCallbackFunction	pointer to EyeImageCallbackFunction
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER			parameter out of range
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetEyeImageCallback (pDLLSetEyeImage pEyeImageCallbackFunction);

/**
 * @brief Sets the customer license (required only for OEM devices!).
 * @sa @listsystemstartstop
 *
 * @param	licenseKey				provided license key
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER		parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetLicense (const char * licenseKey);

/**
 * @brief Defines the logging behavior of iView X SDK.
 *
 * @param	logLevel				log level
 *									multiple log levels can can be combined
 * @param	filename				filename of log file
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER		parameter out of range
 * @retval	ERR_ACCESS_TO_FILE		failed to access log file
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetLogger (int logLevel, char * filename);

/**
 * @brief Define the eye trackers stand alone and monitor integrated geometry.
 * See chapter @ref redgeometry in the iView X SDK Manual
 * and @listgeometry for details.
 *
 * @param	redGeometry					see reference information for @ref REDGeometryStruct
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_PARAMETER			parameter out of range
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function
 *										is not connected
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetREDGeometry (struct REDGeometryStruct * redGeometry);

/**
 * @brief Defines a fixed resolution independent to the screen
 * resolution of chosen display device defined in @ref iV_SetupCalibration function.
 *
 * @param	stimulusWidth			horizontal resolution of stimulus screen [pixel]
 * @param	stimulusHeight			vertical resolution of stimulus screen [pixel]
 *
 * @retval	RET_SUCCESS				intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER		parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetResolution (int stimulusWidth, int stimulusHeight);

/**
 * @brief Sets a callback function for the raw sample data. The function will be called
 * if iView eye tracking server has calculated a new data sample.
 * @attention Algorithms with high processor usage and long calculation time
 * should not run within this callback due to a higher probability of data loss
 * @sa @listdataacquisition
 *
 * @param	pSampleCallbackFunction		pointer to SampleCallbackFunction
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER			parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetSampleCallback (pDLLSetSample pSampleCallbackFunction);

/**
 * @brief Sets a callback function for the scene video image data. The function will be
 * called if a new scene video image is available. The image format is RGB 24bpp.
 * @attention Algorithms with high processor usage and long calculation time
 * should not run within this callback due to a higher probability of data loss
 *
 * @param	pSceneVideoCallbackFunction		pointer to SceneVideoCallbackFunction
 *
 * @retval	RET_SUCCESS						intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER				parameter out of range
 * @retval	ERR_DEPRECATED_FUNCTION	function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetSceneVideoCallback (pDLLSetSceneVideo pSceneVideoCallbackFunction);

/**
 * @brief This function requests the iView eye tracking server to switch the eye tracking frequency to the specified value.
 * Use @ref iV_GetSpeedModes to get the available speed modes for the connected eye tracking device.
 * @attention This fuction is available for SMI iViewRED 4.2 and later versions.
 *
 * @retval RET_SUCCESS			intended functionality has been fulfilled
 * @retval ERR_NOT_CONNECTED	no connection established
 * @retval ERR_WRONG_DEVICE		eye tracking device required for this function is not connected 
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetSpeedMode (int speedMode);

/**
* @brief Sets iView eye tracking server tracking mode. See @ref TrackingMode, @ref trackerParameter subsection and
* iView eye tracking server manual for further explanations.
*
* @attention This function is available with SMI iViewRED 4.4 or later and replaces the @ref iV_SetTrackingParameter function.
*
* @param	mode						selected tracking mode
*
* @retval	RET_SUCCESS					intended functionality has been fulfilled
* @retval	ERR_NOT_CONNECTED			no connection established
* @retval	ERR_WRONG_PARAMETER			parameter out of range
* @retval	ERR_FEATURE_NOT_LICENSED	the requested tracking mode is not licensed
**/
DLL_DEC_PRE int DLL_DEC_POST iV_SetTrackingMode(enum TrackingMode mode);

/**
 * @brief Sets a callback function for the tracking monitor image data. The function will be
 * called if a new tracking monitor image was calculated. The image format is BGR 24bpp.
 * @attention Algorithms with high processor usage and long calculation time
 * should not run within this callback due to a higher probability of data loss
 *
 * @param	pTrackingMonitorCallbackFunction	pointer to TrackingMonitorCallbackFunction
 *
 * @retval	RET_SUCCESS							intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetTrackingMonitorCallback (pDLLSetTrackingMonitor pTrackingMonitorCallbackFunction);

/**
 * @brief Sets iView eye tracking server tracking parameters. See @ref trackerParameter subsection and
 * iView eye tracking server manual for further explanations.
 * Important note: This function can strongly affect tracking stability of your iView X and 
 * eyetracking-server system. Only experienced users should use this function.
 *
 * @param	ET_PARAM_EYE				select specific eye
 * @param	ET_PARAM					select parameter that shall be set
 * @param	value						new value for selected parameter
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_PARAMETER			parameter out of range
 * @retval	ERR_FEATURE_NOT_LICENSED	the features requested by means of
 *										the ET_PARAM parameter is not licensed
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetTrackingParameter (int ET_PARAM_EYE, int ET_PARAM, int value);

/**
 * @brief Sets the calibration and validation visualization parameter.
 * @sa @listcalibration
 *
 * @param	calibrationData					see @ref CalibrationStruct
 *
 * @retval	RET_SUCCESS						intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER				parameter out of range
 * @retval	ERR_WRONG_DEVICE				eye tracking device required for this
 *											function is not connected
 * @retval	ERR_WRONG_CALIBRATION_METHOD	eye tracking device required for this
 *											calibration method is not connected
 * @retval	ERR_FEATURE_NOT_LICENSED		at least one of the features requested by means of
 *											the calibrationData parameters are not licensed
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetupCalibration (struct CalibrationStruct * calibrationData);

/**
 * @brief Enables or disables the debug mode for the current connection. The debug mode disables the automatic
 * connection termination after 5 seconds of an unresponsive server or client. This can happen e.g. during
 * debugging a client application.
 * Beware: the debug mode must not be enabled for production code, as it makes the connection status detection
 * of all API functions unreliable!
 * @attention This fuction is available for SMI iViewRED 4.2 and later versions.
 *
 * @param	enableDebugMode					specifies whether the debug mode shall be disabled (0, default) or enabled (1) 
 *
 * @retval	RET_SUCCESS						intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED				no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetupDebugMode (int enableDebugMode);

/**
 * @brief Enables or disables the LPT signal recording functionality. When enabling the LPT port signal
 * recording the LPT port name has to be specified. The LPT ports available for recording can be queried by
 * using the  @ref iV_GetAvailableLptPorts API function.
 * The function must not be called during a running recording.
 * @attention This fuction is available for SMI iViewRED 4.2 and later versions.
 *
 * @param	portName						a string referencing the LPT port to be used. Appropriate values
 * 											can be queried using the @ref iV_GetAvailableLptPorts function
 * @param	enableRecording					specifies whether recording of LPT port signal shall be enabled
 * 											(1) or disabled (0)
 *
 * @retval	RET_SUCCESS						intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER				the specified port is not available
 * @retval	ERR_IVIEWX_ACCESS_INCOMPLETE	recording in progress
 * @retval	ERR_NOT_CONNECTED				no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetupLptRecording (const char * portName, int enableRecording);

/**
 * @brief Sets and resets the interaction keys during the calibration and validation process.
 * If @p enableKeys is set to 1 (default) all available user interaction keys:
 * - *SPACE* for accepting calibration/validation points
 * - *ESC* for aborting calibration/validation
 * - *TAB* for skipping a point (only SMI iViewRED 4.2 or later)
 *
 * are available during the calibration and the validation process. If @p enableKeys is set to 0 the keys cannot be used.
 * @sa @listvalidation @listcalibration
 *
 * @param	enableKeys						specifies whether calibration keys are enabled
 * 											(1) or disabled (0)
 *
 * @retval	RET_SUCCESS						intended functionality has been fulfilled
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetUseCalibrationKeys (int enableKeys);

/**
 * @brief The validated accuracy results will be visualized in a separate window. Before the
 * image can be drawn the calibration needs to be performed with @ref iV_Calibrate and
 * validated with @ref iV_Validate.
 * @sa @listvalidation
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_NO_VALID_DATA			no data available
 * @retval	RET_WINDOW_IS_OPEN			window is already open
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_NOT_CALIBRATED			system is not calibrated
 * @retval	ERR_NOT_VALIDATED			system is not validated
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ShowAccuracyMonitor ();

/**
 * @brief Visualizes eye image in a separate window while the participant is beeing tracked (equal to image obtained with @ref iV_GetEyeImage).
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_WINDOW_IS_OPEN			window is already open
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function
 *										is not connected
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ShowEyeImageMonitor ();

/**
 * @brief Visualizes scene video in separate window.
 * @attention Only available for HED devices.
 * @sa @listhedscene
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_WINDOW_IS_OPEN			window is already open
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function
 *										is not connected
 * @retval	ERR_DEPRECATED_FUNCTION		function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ShowSceneVideoMonitor ();

/**
 * @brief Visualizes RED tracking monitor in a separate window. It shows the position
 * of the participant related to the eye tracking device and indicates (using arrows)
 * if the participant is not positioned in the center of the tracking tracking box.
 * The image is similar to the one obtained from @ref iV_GetTrackingMonitor.
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	RET_WINDOW_IS_OPEN			window is already open
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function
 *										is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ShowTrackingMonitor ();

/**
 * @brief Starts the iView eye tracking server application. Depending on the PC,
 * it may take several seconds to start the iView eye tracking server application.
 * The connection needs to be established separately using @ref iV_Connect. The connection
 * timeout can be extended using @ref iV_SetConnectionTimeout.
 * @sa @listsystemstartstop
 *
 * @param	etApplication				the iView eye tracking server application which will be started
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_COULD_NOT_CONNECT		failed to establish connection
 * @retval	ERR_IVIEWX_NOT_FOUND		failed to start iViewX application
 * @retval  ERR_CAMERA_NOT_FOUND		no eye tracker connected or the connected eye tracker is not supported
 * @retval  ERR_USB2_CAMERA_PORT		the eye tracker should be connected to a USB 3 port
 * @retval  ERR_USB3_CAMERA_PORT		the eye tracker should be connected to a USB 2 port
 * @retval  ERR_MULTIPLE_DEVICES		multiple eye trackers are detected but only one is expected
 * @retval  ERR_WRONG_DEVICE			the connected eye tracker is not supported
 * @retval	ERR_LICENSE_EXPIRED			the eye tracker license has expired
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Start (enum ETApplication etApplication);

/**
 * @brief Starts gaze data recording and scene video recording (if connected eye tracking
 * device is *HED*). @ref iV_StartRecording does not return until gaze and scene video
 * recording is started. The data streaming needs to be stopped by using @ref iV_StopRecording
 * before it can be saved using @ref iV_SaveData.
 * @sa @listeyedatarecording
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this
 *										function is not connected
 * @retval	ERR_RECORDING_DATA_BUFFER	recording is activated
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_StartRecording ();

/**
 * @brief Stops gaze data recording and scene video recording (if connected eye tracking
 * device is *HED*). @ref iV_StopRecording does not return until gaze and scene video
 * recording is stopped. This function needs to be called before the data can
 * be saved using @ref iV_SaveData.
 * @sa @listeyedatarecording
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function
 *										is not connected
 * @retval	ERR_EMPTY_DATA_BUFFER		recording buffer is empty
 * @retval	ERR_FEATURE_NOT_LICENSED	feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_StopRecording ();

/**
 * @brief Sends a TTL value to defined port. Define a port with @ref iV_DefineAOIPort.
 * @sa @listaoi
 *
 * @param	value						value which will be sends out as TTL signal
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_WRONG_PARAMETER			parameter out of range
 * @retval	ERR_DEPRECATED_FUNCTION		function is not available in SMI iViewRED 4.2 and later
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_TestTTL (int value);

/**
* @brief Starts a validation procedure. To proceed, the participant needs to be tracked and
* has to fixate the validation point. Depending on the validation settings (which can
* be changed using @ref iV_SetupCalibration and @ref iV_SetUseCalibrationKeys) the user
* can accept the validation points manually (by pressing [SPACE] or calling @ref iV_AcceptCalibrationPoint) or abort the
* validation (by pressing [ESC] or calling @ref iV_AbortCalibration)
*
* If the validation is visualized by the API (@ref CalibrationStruct::visualization is set to 1)
* the function will not return until the validation has been finished (closed automatically)
* or aborted (by using [ESC]).
*
* If the @ref CalibrationStruct::visualization is set to 0, the function call returns
* immediately. The user has to implement the visualization of validation points. Information
* about the current validation point can be retrieved with @ref iV_GetCurrentCalibrationPoint
* or with setting up the calibration callback using @ref iV_SetCalibrationCallback.
 * @sa @listvalidation @listcalibration
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_NOT_CALIBRATED			system is not calibrated
 * @retval	ERR_WRONG_DEVICE			eye tracking device required for this function is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Validate ();

/** @}*/

/**
 * @struct REDStandAloneModeStruct
 *
 * @brief Deprecated. Please use @ref REDGeometryStruct instead.
 */
struct REDStandAloneModeStruct {
		//! horizontal stimulus calibration size [mm]
		int stimX;

		//! vertical stimulus calibration size [mm]
		int stimY;

		//! distance floor to stimulus screen [mm]
		int stimHeightOverFloor;

		//! distance floor to RED [mm]
		int redHeightOverFloor;

		//! distance RED to stimulus screen [mm]
		int redStimDist;

		//! RED inclination angle [degree]
		int redInclAngle;
};

/**
 * @struct REDMonitorAttachedGeometryStruct
 *
 * @brief Deprecated. Please use @ref REDGeometryStruct instead.
 */
struct REDMonitorAttachedGeometryStruct {
		//! horizontal stimulus calibration size [mm]
		int stimX;

		//! vertical stimulus calibration size [mm]
		int stimY;

		//! vertical distance RED-m to stimulus screen [mm]
		int redStimDistHeight;

		//! horizontal distance RED-m to stimulus screen [mm]
		int redStimDistDepth;

		//! RED-m inclination angle [degree]
		int redInclAngle;
};

/**
 * @brief Deprecated. Please use @ref iV_SetREDGeometry instead.
 **/
DEPRECATED DLL_DEC_PRE int DLL_DEC_POST iV_SetupMonitorAttachedGeometry (
        struct MonitorAttachedGeometryStruct * monitorAttachedGeometry);

/**
 * @brief Deprecated. Please use @ref iV_SetREDGeometry instead.
 **/
DEPRECATED DLL_DEC_PRE int DLL_DEC_POST iV_SetupStandAloneMode (
        struct StandAloneModeGeometryStruct * standAloneModeGeometry);

/**
 * @brief Deprecated. Please use @ref iV_SetREDGeometry instead.
 **/
DEPRECATED DLL_DEC_PRE int DLL_DEC_POST iV_SetupREDMonitorAttachedGeometry (
        struct REDMonitorAttachedGeometryStruct * attachedModeGeometry);

/**
 * @brief Deprecated. Please use @ref iV_SetREDGeometry instead.
 **/
DEPRECATED DLL_DEC_PRE int DLL_DEC_POST iV_SetupREDStandAloneMode (
        struct REDStandAloneModeStruct * standAloneModeGeometry);

/**
 * @brief Deprecated. Please use @ref iV_GetREDGeometry instead.
 **/
DEPRECATED DLL_DEC_PRE int DLL_DEC_POST iV_GetMonitorAttachedGeometry (char profileName[256],
        struct MonitorAttachedGeometryStruct * monitorAttachedGeometry);

/**
 * @brief Deprecated. Please use @ref iV_SelectREDGeometry instead.
 **/
DEPRECATED DLL_DEC_PRE int DLL_DEC_POST iV_SetGeometryProfile (char * profileName);

/**
 * @brief Deprecated. Please use @ref iV_DeleteREDGeometry instead.
 **/
DEPRECATED DLL_DEC_PRE int DLL_DEC_POST iV_DeleteMonitorAttachedGeometry (char setupName[256]);

/**
 * @brief Deprecated. Please use @ref iV_DeleteREDGeometry instead.
 **/
DEPRECATED DLL_DEC_PRE int DLL_DEC_POST iV_DeleteStandAloneGeometry (char setupName[256]);

