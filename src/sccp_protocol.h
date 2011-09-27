
/*!
 * \file 	sccp_protocol.h
 * \brief 	SCCP Protocol Header
 * \author 	Sergio Chersovani <mlists [at] c-net.it>
 * \note	Reworked, but based on chan_sccp code.
 *        	The original chan_sccp driver that was made by Zozo which itself was derived from the chan_skinny driver.
 *        	Modified by Jan Czmok and Julien Goodwin
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *		See the LICENSE file at the top of the source tree.
 * 
 * \note 	Thank you Federico Santulli for SPCP support
 *
 * $Date$
 * $Revision$  
 */

#ifndef __SCCP_PROTOCOL_H
#    define __SCCP_PROTOCOL_H

#    include "sccp_labels.h"

#    define SCCP_DRIVER_SUPPORTED_PROTOCOL_LOW	3				/*!< At least we require protocol V.3 */
#    define SCCP_DRIVER_SUPPORTED_PROTOCOL_HIGH	20				/*!< We support up to protocol V.17 */

#    define SCCP_PROTOCOL	0
#    define SPCP_PROTOCOL	1

#    define DEFAULT_SCCP_PORT			2000				/*!< SCCP uses port 2000. */
#    define DEFAULT_SCCP_BACKLOG			2			/*!< the listen baklog. */
#    define SCCP_MAX_AUTOLOGIN			100				/*!< Maximum allowed of autologins per device */
#    define SCCP_KEEPALIVE				5			/*!< Default keepalive time if not specified in sccp.conf. */

#    define SKINNY_PHONE_FEATUES_PROTOCOLVERSION 0xFF
#    define SKINNY_PHONE_FEATUES_ABBRDIAL 1<<31
#    define SKINNY_PHONE_FEATUES_DYNAMIC_MESSAGES 1<<24

/*!
 * \brief Skinny Device Feature (ENUM)
 */
typedef enum {
	SKINNY_DEVFEATURE_DYNAMIC_MESSAGE = 1 << 24,
	SKINNY_DEVFEATURE_ABBREVIATED_DIAL = 1 << 31,
} skinny_devfeature_t;								/*!< Skinny Device Feature Enum */


/*! 
 * \brief internal chan_sccp call state (c->callstate) (Enum)
 */
typedef enum {
	SCCP_CHANNELSTATE_DOWN = 0,
	SCCP_CHANNELSTATE_OFFHOOK = 1,
	SCCP_CHANNELSTATE_ONHOOK = 2,
	SCCP_CHANNELSTATE_RINGOUT = 3,
	SCCP_CHANNELSTATE_RINGING = 4,
	SCCP_CHANNELSTATE_CONNECTED = 5,
	SCCP_CHANNELSTATE_BUSY = 6,
	SCCP_CHANNELSTATE_CONGESTION = 7,
	SCCP_CHANNELSTATE_HOLD = 8,
	SCCP_CHANNELSTATE_CALLWAITING = 9,
	SCCP_CHANNELSTATE_CALLTRANSFER = 10,
	SCCP_CHANNELSTATE_CALLPARK = 11,
	SCCP_CHANNELSTATE_PROCEED = 12,
	SCCP_CHANNELSTATE_CALLREMOTEMULTILINE = 13,
	SCCP_CHANNELSTATE_INVALIDNUMBER = 14,
	SCCP_CHANNELSTATE_DIALING = 20,
	SCCP_CHANNELSTATE_PROGRESS = 21,
	SCCP_CHANNELSTATE_GETDIGITS = 0xA0,
	SCCP_CHANNELSTATE_CALLCONFERENCE = 0xA1,
	SCCP_CHANNELSTATE_SPEEDDIAL = 0xA2,
	SCCP_CHANNELSTATE_DIGITSFOLL = 0xA3,
	SCCP_CHANNELSTATE_INVALIDCONFERENCE = 0xA4,
	SCCP_CHANNELSTATE_CONNECTEDCONFERENCE = 0xA5,
	SCCP_CHANNELSTATE_BLINDTRANSFER = 0xA6,
	SCCP_CHANNELSTATE_ZOMBIE = 0xFE,
	SCCP_CHANNELSTATE_DND = 0xFF
} sccp_channelState_t;								/*!< internal Chan_SCCP Call State c->callstate */

/*!
 * \brief SCCP ChannelState Structure
 */
static const struct sccp_channelstate {
	sccp_channelState_t channelstate;
	const char *const text;
} sccp_channelstates[] = {
	/* *INDENT-OFF* */
	{SCCP_CHANNELSTATE_DOWN, "DOWN"},
	{SCCP_CHANNELSTATE_OFFHOOK, "OFFHOOK"},
	{SCCP_CHANNELSTATE_ONHOOK, "ONHOOK"},
	{SCCP_CHANNELSTATE_RINGOUT, "RINGOUT"},
	{SCCP_CHANNELSTATE_RINGING, "RINGING"},
	{SCCP_CHANNELSTATE_CONNECTED, "CONNECTED"},
	{SCCP_CHANNELSTATE_BUSY, "BUSY	"},
	{SCCP_CHANNELSTATE_CONGESTION, "CONGESTION"},
	{SCCP_CHANNELSTATE_HOLD, "HOLD	"},
	{SCCP_CHANNELSTATE_CALLWAITING, "CALLWAITING"},
	{SCCP_CHANNELSTATE_CALLTRANSFER, "CALLTRANSFER"},
	{SCCP_CHANNELSTATE_CALLPARK, "CALLPARK"},
	{SCCP_CHANNELSTATE_PROCEED, "PROCEED"},
	{SCCP_CHANNELSTATE_CALLREMOTEMULTILINE, "CALLREMOTEMULTILINE"},
	{SCCP_CHANNELSTATE_INVALIDNUMBER, "INVALIDNUMBER"},
	{SCCP_CHANNELSTATE_DIALING, "DIALING"},
	{SCCP_CHANNELSTATE_PROGRESS, "PROGRESS"},
	{SCCP_CHANNELSTATE_GETDIGITS, "GETDIGITS"},
	{SCCP_CHANNELSTATE_CALLCONFERENCE, "CALLCONFERENCE"},
	{SCCP_CHANNELSTATE_SPEEDDIAL, "SPEEDDIAL"},
	{SCCP_CHANNELSTATE_DIGITSFOLL, "DIGITSFOLL"},
	{SCCP_CHANNELSTATE_INVALIDCONFERENCE, "INVALIDCONFERENCE"},
	{SCCP_CHANNELSTATE_CONNECTEDCONFERENCE, "CONNECTEDCONFERENCE"},
	{SCCP_CHANNELSTATE_BLINDTRANSFER, "BLINDTRANSFER"},
	{SCCP_CHANNELSTATE_ZOMBIE, "ZOMBIE"},
	{SCCP_CHANNELSTATE_DND, "DND"}
	/* *INDENT-ON* */
};

/*!
 * \brief Skinny Miscellaneous Command Type (Enum)
 */
typedef enum {
	SKINNY_MISCCOMMANDTYPE_VIDEOFREEZEPICTURE = 0,
	SKINNY_MISCCOMMANDTYPE_VIDEOFASTUPDATEPICTURE,
	SKINNY_MISCCOMMANDTYPE_VIDEOFASTUPDATEGOB,
	SKINNY_MISCCOMMANDTYPE_VIDEOFASTUPDATEMB,
	SKINNY_MISCCOMMANDTYPE_LOSTPICTURE,
	SKINNY_MISCCOMMANDTYPE_LOSTPARTIALPICTURE,
	SKINNY_MISCCOMMANDTYPE_RECOVERYREFERENCEPICTURE,
	SKINNY_MISCCOMMANDTYPE_TEMPORALSPATIALTRADEOFF
} sccp_miscCommandType_t;							/*!< misccommand types */

/*
static const value_string skinny_miscCommandType[] = {
	{0  , "videoFreezePicture"},
	{1  , "videoFastUpdatePicture"},
	{2  , "videoFastUpdateGOB"},
	{3  , "videoFastUpdateMB"},
	{4  , "lostPicture"},
	{5  , "lostPartialPicture"},
	{6  , "recoveryReferencePicture"},
	{7  , "temporalSpatialTradeOff"},
	{0  , NULL}
};
*/

/*
 static const value_string skinny_formatTypes[] = {
 {1  , "sqcif (128x96)"},
 {2  , "qcif (176x144)"},
 {3  , "cif (352x288)"},
 {4  , "4cif (704x576)"},
 {5  , "16cif (1408x1152)"},
 {6  , "custom_base"},
 {0  , NULL}
 };
*/

/*!
 * \brief AST Device State Structure
 */
static const struct pbx_devicestate {
#    ifdef ENUM_AST_DEVICE
	enum ast_device_state devicestate;
#    else
	uint8_t devicestate;
#    endif
	const char *const text;
} pbx_devicestates[] = {
	/* *INDENT-OFF* */
	{AST_DEVICE_UNKNOWN, "Device is valid but channel doesn't know state"},
	{AST_DEVICE_NOT_INUSE, "Device is not in use"},
	{AST_DEVICE_INUSE, "Device is in use"},
	{AST_DEVICE_BUSY, "Device is busy"},
	{AST_DEVICE_INVALID, "Device is invalid"},
	{AST_DEVICE_UNAVAILABLE, "Device is unavailable"},
	{AST_DEVICE_RINGING, "Device is ringing"},
	{AST_DEVICE_RINGINUSE, "Device is ringing and in use"},
	{AST_DEVICE_ONHOLD, "Device is on hold"},
#    ifdef AST_DEVICE_TOTAL
	{AST_DEVICE_TOTAL, "Total num of device states, used for testing"}
#    endif
	/* *INDENT-ON* */
};

/*!
 * \brief SCCP BLF States for Dynamic Speeddials (ENUM)
 */
typedef enum {
	SCCP_BLF_STATUS_UNKNOWN = 0,						/*!< unknown status - default */
	SCCP_BLF_STATUS_IDLE = 1,						/*!< not in use */
	SCCP_BLF_STATUS_INUSE = 2,						/*!< in use */
	SCCP_BLF_STATUS_DND = 3,						/*!< dnd */
	SCCP_BLF_STATUS_ALERTING = 4						/*!< alerting */
} sccp_BFLState_t;								/*!< blf states for dynamic speeddials */

#    define SCCP_CFWD_NONE				0
#    define SCCP_CFWD_ALL				1
#    define SCCP_CFWD_BUSY				2
#    define SCCP_CFWD_NOANSWER			3

/*!
 * \brief SCCP CallForwardState Structure
 */
static const struct sccp_callforwardstate {
	uint8_t callforwardstate;
	const char *const text;
	const char *const longtext;
} sccp_callforwardstates[] = {
	/* *INDENT-OFF* */
	{SCCP_CFWD_NONE, "None", "No Call Forward"},
	{SCCP_CFWD_ALL, "All", "All Calls are forwarded"},
	{SCCP_CFWD_BUSY, "Busy", "Call is forwarded when busy"}, 
	{SCCP_CFWD_NOANSWER, "NoAnswer", "Call is forwarded when no-answer"},
	/* *INDENT-ON* */
};

/*
 * SCCP/Skinny Lookup Types
 */
#    define SCCP_MESSAGE 		0
#    define SCCP_ACCESSORY 		1
#    define SCCP_ACCESSORY_STATE	2
#    define SCCP_EXTENSION_STATE	3
#    define SCCP_DNDMODE		4
#    define SKINNY_TONE 		5
#    define SKINNY_ALARM		6
#    define SKINNY_DEVICETYPE	7
#    define SKINNY_DEVICE_RS	8
#    define SKINNY_STIMULUS		9
#    define SKINNY_BUTTONTYPE	10
#    define SKINNY_LAMPMODE		11
#    define SKINNY_STATION		12
#    define SKINNY_LBL		13
#    define SKINNY_CALLTYPE		14
#    define SKINNY_KEYMODE		15
#    define SKINNY_DEVICE_STATE	16
#    define SKINNY_CODEC		17

/*!
 * \brief Skinny Protocol Call States (ENUM)
 */
typedef enum {
	SKINNY_CALLSTATE_OFFHOOK = 1,
	SKINNY_CALLSTATE_ONHOOK = 2,
	SKINNY_CALLSTATE_RINGOUT = 3,
	SKINNY_CALLSTATE_RINGIN = 4,
	SKINNY_CALLSTATE_CONNECTED = 5,
	SKINNY_CALLSTATE_BUSY = 6,
	SKINNY_CALLSTATE_CONGESTION = 7,
	SKINNY_CALLSTATE_HOLD = 8,
	SKINNY_CALLSTATE_CALLWAITING = 9,
	SKINNY_CALLSTATE_CALLTRANSFER = 10,
	SKINNY_CALLSTATE_CALLPARK = 11,
	SKINNY_CALLSTATE_PROCEED = 12,
	SKINNY_CALLSTATE_CALLREMOTEMULTILINE = 13,
	SKINNY_CALLSTATE_INVALIDNUMBER = 14,
	SKINNY_CALLSTATE_HOLDYELLOW = 15,
	SKINNY_CALLSTATE_INTERCOMONEWAY = 16,
	SKINNY_CALLSTATE_HOLDRED = 17
} skinny_callstate_t;

/*!
 * \brief Skinny Protocol Call Type (ENUM)
 */
typedef enum {
	SKINNY_CALLTYPE_INBOUND = 1,
	SKINNY_CALLTYPE_OUTBOUND = 2,
	SKINNY_CALLTYPE_FORWARD = 3
} skinny_calltype_t;								/*!< Skinny Calltype */

/*!
 * \brief Skinny CallType Structure
 */
static const struct skinny_calltype {
	skinny_calltype_t calltype;
	const char *const text;
} skinny_calltypes[] = {
	/* *INDENT-OFF* */
	{SKINNY_CALLTYPE_INBOUND, "Inbound"},
	{SKINNY_CALLTYPE_OUTBOUND, "Outbound"}, 
	{SKINNY_CALLTYPE_FORWARD, "Forward"},
	/* *INDENT-ON* */
};


/*!
 * \brief Skinny Device Feature Structure
 */
static const struct skinny_devfeature {
	skinny_devfeature_t device_feature;
	const char *const text;
} skinny_devfeatures[] = {
	/* *INDENT-OFF* */
	{SKINNY_DEVFEATURE_DYNAMIC_MESSAGE, "Dynamic Messages"}, 
	{SKINNY_DEVFEATURE_ABBREVIATED_DIAL, "Abbreviated Dial"},
	/* *INDENT-ON* */
};

/*!
 * \brief Skinny Call Priority (ENUM)
 */
typedef enum {
	SKINNY_CALLPRIORITY_HIGHEST = 0,
	SKINNY_CALLPRIORITY_HIGH = 1,
	SKINNY_CALLPRIORITY_MEDIUM = 2,
	SKINNY_CALLPRIORITY_LOW = 3,
	SKINNY_CALLPRIORITY_NORMAL = 4
} skinny_callPriority_t;							/*!< Skinny Call Priority */

/*!
 * \brief Skinny Call Visibility (ENUM)
 */
typedef enum {
	SKINNY_CALLINFO_VISIBILITY_DEFAULT = 0,
	SKINNY_CALLINFO_VISIBILITY_COLLAPSED = 1,
	SKINNY_CALLINFO_VISIBILITY_HIDDEN = 2
} skinny_callinfo_visibility_t;							/*!< Skinny Call Info Visibility */

/*!
 * \brief Skinny Call Security (ENUM)
 */
typedef enum {
	SKINNY_CALLSECURITYSTATE_UNKNOWN = 0,
	SKINNY_CALLSECURITYSTATE_NOTAUTHENTICATED = 1,
	SKINNY_CALLSECURITYSTATE_AUTHENTICATED = 2
} skinny_callstate_security_t;							/*!< Skinny CallSecurityInfo */

#    define StationMaxDeviceNameSize		16
#    define StationMaxButtonTemplateSize	56
#    define StationMaxButtonTemplateNameSize	44
#    define StationDateTemplateSize 		6
//#define StationMaxDisplayTextSize             33
#    define StationMaxDisplayTextSize		32
#    define StationMaxDisplayNotifySize 		32
#    define StationMaxDirnumSize			24
#    define StationMaxNameSize			40
#    define StationMaxSoftKeyDefinition 		32
#    define StationMaxSoftKeySetDefinition		16
#    define StationMaxSoftKeyIndex			16
#    define StationMaxSoftKeyLabelSize		16
#    define StationMaxVersionSize			16
#    define StationMaxServers			5
#    define ServerMaxNameSize			48
#    define StationMaxServiceURLSize		256
#    define StationMaxPorts				16
#    define StationMaxXMLMessage			2048

#    define APPID_CONFERENCE				1
#    define APPID_PROVISION				2

/* skinny tones skinny_tone2str */
#    define SKINNY_TONE_SILENCE 		0
#    define SKINNY_TONE_DTMF1			1
#    define SKINNY_TONE_DTMF2			2
#    define SKINNY_TONE_DTMF3			3
#    define SKINNY_TONE_DTMF4			4
#    define SKINNY_TONE_DTMF5			5
#    define SKINNY_TONE_DTMF6			6
#    define SKINNY_TONE_DTMF7			7
#    define SKINNY_TONE_DTMF8			8
#    define SKINNY_TONE_DTMF9			9
#    define SKINNY_TONE_DTMF0			0xa
#    define SKINNY_TONE_DTMFSTAR			0xe
#    define SKINNY_TONE_DTMFPOUND			0xf
#    define SKINNY_TONE_DTMFA			0x10
#    define SKINNY_TONE_DTMFB			0x11
#    define SKINNY_TONE_DTMFC			0x12
#    define SKINNY_TONE_DTMFD			0x13
#    define SKINNY_TONE_INSIDEDIALTONE		0x21
#    define SKINNY_TONE_OUTSIDEDIALTONE 		0x22
#    define SKINNY_TONE_LINEBUSYTONE		0x23
#    define SKINNY_TONE_ALERTINGTONE		0x24
#    define SKINNY_TONE_REORDERTONE 		0x25
#    define SKINNY_TONE_RECORDERWARNINGTONE 	0x26
#    define SKINNY_TONE_RECORDERDETECTEDTONE	0x27
#    define SKINNY_TONE_REVERTINGTONE		0x28
#    define SKINNY_TONE_RECEIVEROFFHOOKTONE 	0x29
#    define SKINNY_TONE_PARTIALDIALTONE 		0x2a
#    define SKINNY_TONE_NOSUCHNUMBERTONE		0x2b
#    define SKINNY_TONE_BUSYVERIFICATIONTONE	0x2c
#    define SKINNY_TONE_CALLWAITINGTONE 		0x2d
#    define SKINNY_TONE_CONFIRMATIONTONE		0x2e
#    define SKINNY_TONE_CAMPONINDICATIONTONE	0x2f
#    define SKINNY_TONE_RECALLDIALTONE		0x30
#    define SKINNY_TONE_ZIPZIP			0x31
#    define SKINNY_TONE_ZIP 			0x32
#    define SKINNY_TONE_BEEPBONK			0x33
#    define SKINNY_TONE_MUSICTONE			0x34
#    define SKINNY_TONE_HOLDTONE			0x35
#    define SKINNY_TONE_TESTTONE			0x36
#    define SKINNY_TONE_DTMONITORWARNINGTONE	0x37
#    define SKINNY_TONE_ADDCALLWAITING		0x40
#    define SKINNY_TONE_PRIORITYCALLWAIT		0x41
#    define SKINNY_TONE_RECALLDIAL			0x42
#    define SKINNY_TONE_BARGIN			0x43
#    define SKINNY_TONE_DISTINCTALERT		0x44
#    define SKINNY_TONE_PRIORITYALERT		0x45
#    define SKINNY_TONE_REMINDERRING		0x46
#    define SKINNY_TONE_PRECEDENCE_RINGBACK		0x47
#    define SKINNY_TONE_PREEMPTIONTONE		0x48
#    define SKINNY_TONE_MF1 			0x50
#    define SKINNY_TONE_MF2 			0x51
#    define SKINNY_TONE_MF3 			0x52
#    define SKINNY_TONE_MF4 			0x53
#    define SKINNY_TONE_MF5 			0x54
#    define SKINNY_TONE_MF6 			0x55
#    define SKINNY_TONE_MF7 			0x56
#    define SKINNY_TONE_MF8 			0x57
#    define SKINNY_TONE_MF9 			0x58
#    define SKINNY_TONE_MF0 			0x59
#    define SKINNY_TONE_MFKP1			0x5a
#    define SKINNY_TONE_MFST			0x5b
#    define SKINNY_TONE_MFKP2			0x5c
#    define SKINNY_TONE_MFSTP			0x5d
#    define SKINNY_TONE_MFST3P			0x5e
#    define SKINNY_TONE_MILLIWATT			0x5f
#    define SKINNY_TONE_MILLIWATTTEST		0x60
#    define SKINNY_TONE_HIGHTONE			0x61
#    define SKINNY_TONE_FLASHOVERRIDE		0x62
#    define SKINNY_TONE_FLASH			0x63
#    define SKINNY_TONE_PRIORITY			0x64
#    define SKINNY_TONE_IMMEDIATE			0x65
#    define SKINNY_TONE_PREAMPWARN			0x66
#    define SKINNY_TONE_2105HZ			0x67
#    define SKINNY_TONE_2600HZ			0x68
#    define SKINNY_TONE_440HZ			0x69
#    define SKINNY_TONE_300HZ			0x6a
#    define SKINNY_TONE_MLPP_PALA			0x77
#    define SKINNY_TONE_MLPP_ICA			0x78
#    define SKINNY_TONE_MLPP_VCA			0x79
#    define SKINNY_TONE_MLPP_BPA			0x7A
#    define SKINNY_TONE_MLPP_BNEA			0x7B
#    define SKINNY_TONE_MLPP_UPA			0x7C
#    define SKINNY_TONE_NOTONE			0x7f

/*!
 * \brief Skinny Tone Structure
 */
static const struct skinny_tone {
	uint8_t tone;
	const char *const text;
} skinny_tones[] = {
	/* *INDENT-OFF* */
	{SKINNY_TONE_SILENCE, "Silence"},
	{SKINNY_TONE_DTMF1, "DTMF 1"},
	{SKINNY_TONE_DTMF2, "DTMF 2"},
	{SKINNY_TONE_DTMF3, "DTMF 3"},
	{SKINNY_TONE_DTMF4, "DTMF 4"},
	{SKINNY_TONE_DTMF5, "DTMF 5"},
	{SKINNY_TONE_DTMF6, "DTMF 6"},
	{SKINNY_TONE_DTMF7, "DTMF 7"},
	{SKINNY_TONE_DTMF8, "DTMF 8"},
	{SKINNY_TONE_DTMF9, "DTMF 9"},
	{SKINNY_TONE_DTMF0, "DTMF 0"},
	{SKINNY_TONE_DTMFSTAR, "DTMF Star"},
	{SKINNY_TONE_DTMFPOUND, "DTMF Pound"},
	{SKINNY_TONE_DTMFA, "DTMF A"},
	{SKINNY_TONE_DTMFB, "DTMF B"},
	{SKINNY_TONE_DTMFC, "DTMF C"},
	{SKINNY_TONE_DTMFD, "DTMF D"},
	{SKINNY_TONE_INSIDEDIALTONE, "Inside Dial Tone"},
	{SKINNY_TONE_OUTSIDEDIALTONE, "Outside Dial Tone"},
	{SKINNY_TONE_LINEBUSYTONE, "Line Busy Tone"},
	{SKINNY_TONE_ALERTINGTONE, "Alerting Tone"},
	{SKINNY_TONE_REORDERTONE, "Reorder Tone"},
	{SKINNY_TONE_RECORDERWARNINGTONE, "Recorder Warning Tone"},
	{SKINNY_TONE_RECORDERDETECTEDTONE, "Recorder Detected Tone"},
	{SKINNY_TONE_REVERTINGTONE, "Reverting Tone"},
	{SKINNY_TONE_RECEIVEROFFHOOKTONE, "Receiver OffHook Tone"},
	{SKINNY_TONE_PARTIALDIALTONE, "Partial Dial Tone"},
	{SKINNY_TONE_NOSUCHNUMBERTONE, "No Such Number Tone"},
	{SKINNY_TONE_BUSYVERIFICATIONTONE, "Busy Verification Tone"},
	{SKINNY_TONE_CALLWAITINGTONE, "Call Waiting Tone"},
	{SKINNY_TONE_CONFIRMATIONTONE, "Confirmation Tone"},
	{SKINNY_TONE_CAMPONINDICATIONTONE, "Camp On Indication Tone"},
	{SKINNY_TONE_RECALLDIALTONE, "Recall Dial Tone"},
	{SKINNY_TONE_ZIPZIP, "Zip Zip"},
	{SKINNY_TONE_ZIP, "Zip"},
	{SKINNY_TONE_BEEPBONK, "Beep Bonk"},
	{SKINNY_TONE_MUSICTONE, "Music Tone"},
	{SKINNY_TONE_HOLDTONE, "Hold Tone"},
	{SKINNY_TONE_TESTTONE, "Test Tone"},
	{SKINNY_TONE_DTMONITORWARNINGTONE, "DT Monitor Warning Tone"},
	{SKINNY_TONE_ADDCALLWAITING, "Add Call Waiting"},
	{SKINNY_TONE_PRIORITYCALLWAIT, "Priority Call Wait"},
	{SKINNY_TONE_RECALLDIAL, "Recall Dial"},
	{SKINNY_TONE_BARGIN, "Barg In"},
	{SKINNY_TONE_DISTINCTALERT, "Distinct Alert"},
	{SKINNY_TONE_PRIORITYALERT, "Priority Alert"},
	{SKINNY_TONE_REMINDERRING, "Reminder Ring"},
	{SKINNY_TONE_PRECEDENCE_RINGBACK, "Precedence RingBank"},
	{SKINNY_TONE_PREEMPTIONTONE, "Pre-EmptionTone"},
	{SKINNY_TONE_MF1, "MF1"},
	{SKINNY_TONE_MF2, "MF2"},
	{SKINNY_TONE_MF3, "MF3"},
	{SKINNY_TONE_MF4, "MF4"},
	{SKINNY_TONE_MF5, "MF5"},
	{SKINNY_TONE_MF6, "MF6"},
	{SKINNY_TONE_MF7, "MF7"},
	{SKINNY_TONE_MF8, "MF8"},
	{SKINNY_TONE_MF9, "MF9"},
	{SKINNY_TONE_MF0, "MF0"},
	{SKINNY_TONE_MFKP1, "MFKP1"},
	{SKINNY_TONE_MFST, "MFST"},
	{SKINNY_TONE_MFKP2, "MFKP2"},
	{SKINNY_TONE_MFSTP, "MFSTP"},
	{SKINNY_TONE_MFST3P, "MFST3P"},
	{SKINNY_TONE_MILLIWATT, "MILLIWATT"},
	{SKINNY_TONE_MILLIWATTTEST, "MILLIWATT TEST"},
	{SKINNY_TONE_HIGHTONE, "HIGH TONE"},
	{SKINNY_TONE_FLASHOVERRIDE, "FLASH OVERRIDE"},
	{SKINNY_TONE_FLASH, "FLASH"},
	{SKINNY_TONE_PRIORITY, "PRIORITY"},
	{SKINNY_TONE_IMMEDIATE, "IMMEDIATE"},
	{SKINNY_TONE_PREAMPWARN, "PRE-AMP WARN"},
	{SKINNY_TONE_2105HZ, "2105 HZ"},
	{SKINNY_TONE_2600HZ, "2600 HZ"},
	{SKINNY_TONE_440HZ, "440 HZ"},
	{SKINNY_TONE_300HZ, "300 HZ"},
	{SKINNY_TONE_MLPP_PALA, "MLPP Pala"},
	{SKINNY_TONE_MLPP_ICA, "MLPP Ica"},
	{SKINNY_TONE_MLPP_VCA, "MLPP Vca"},
	{SKINNY_TONE_MLPP_BPA, "MLPP Bpa"},
	{SKINNY_TONE_MLPP_BNEA, "MLPP Bnea"},
	{SKINNY_TONE_MLPP_UPA, "MLPP Upa"}, 
	{SKINNY_TONE_NOTONE, "No Tone"},
	/* *INDENT-ON* */
};

/* alarm skinny_alarm2str*/
#    define SKINNY_ALARM_CRITICAL			0
#    define SKINNY_ALARM_WARNING			1
#    define SKINNY_ALARM_INFORMATIONAL		2
#    define SKINNY_ALARM_UNKNOWN			4
#    define SKINNY_ALARM_MAJOR			7
#    define SKINNY_ALARM_MINOR			8
#    define SKINNY_ALARM_MARGINAL			10
#    define SKINNY_ALARM_TRACEINFO			20

/*!
 * \brief Skinny Alarm Structure
 */
static const struct skinny_alarm {
	uint8_t alarm;
	const char *const text;
} skinny_alarms[] = {
	/* *INDENT-OFF* */
	{SKINNY_ALARM_CRITICAL,		"Critical"},
	{SKINNY_ALARM_WARNING,		"Warning"},
	{SKINNY_ALARM_INFORMATIONAL,	"Informational"},
	{SKINNY_ALARM_UNKNOWN,		"Unknown"},
	{SKINNY_ALARM_MAJOR,		"Major"},
	{SKINNY_ALARM_MINOR,		"Minor"},
	{SKINNY_ALARM_MARGINAL,		"Marginal"}, 
	{SKINNY_ALARM_TRACEINFO,	"TraceInfo"},
	/* *INDENT-ON* */
};

/* devices type */
#    define SKINNY_DEVICETYPE_UNDEFINED			0
#    define SKINNY_DEVICETYPE_30SPPLUS			1
#    define SKINNY_DEVICETYPE_12SPPLUS			2
#    define SKINNY_DEVICETYPE_12SP			3
#    define SKINNY_DEVICETYPE_12			4
#    define SKINNY_DEVICETYPE_30VIP 			5
//#define SKINNY_DEVICETYPE_TELECASTER                  6
//#define SKINNY_DEVICETYPE_TELECASTER_MGR              7
//#define SKINNY_DEVICETYPE_TELECASTER_BUS              8
//#define SKINNY_DEVICETYPE_POLYCOM                     9
#    define SKINNY_DEVICETYPE_VGC			10
#    define SKINNY_DEVICETYPE_ATA186			12
#    define SKINNY_DEVICETYPE_ATA188			12			/*!< ATA188 -FS */
#    define SKINNY_DEVICETYPE_VIRTUAL30SPPLUS		20
#    define SKINNY_DEVICETYPE_PHONEAPPLICATION		21
#    define SKINNY_DEVICETYPE_ANALOGACCESS		30
#    define SKINNY_DEVICETYPE_DIGITALACCESSPRI		40
#    define SKINNY_DEVICETYPE_DIGITALACCESST1		41
#    define SKINNY_DEVICETYPE_DIGITALACCESSTITAN2	42
#    define SKINNY_DEVICETYPE_DIGITALACCESSLENNON	43
#    define SKINNY_DEVICETYPE_ANALOGACCESSELVIS 	47
#    define SKINNY_DEVICETYPE_CONFERENCEBRIDGE		50
#    define SKINNY_DEVICETYPE_CONFERENCEBRIDGEYOKO	51
#    define SKINNY_DEVICETYPE_CONFERENCEBRIDGEDIXIELAND	52
#    define SKINNY_DEVICETYPE_CONFERENCEBRIDGESUMMIT	53
#    define SKINNY_DEVICETYPE_H225			60
#    define SKINNY_DEVICETYPE_H323PHONE 		61
#    define SKINNY_DEVICETYPE_H323TRUNK 		62
#    define SKINNY_DEVICETYPE_MUSICONHOLD		70
#    define SKINNY_DEVICETYPE_PILOT 			71
#    define SKINNY_DEVICETYPE_TAPIPORT			72
#    define SKINNY_DEVICETYPE_TAPIROUTEPOINT		73
#    define SKINNY_DEVICETYPE_VOICEINBOX		80
#    define SKINNY_DEVICETYPE_VOICEINBOXADMIN		81
#    define SKINNY_DEVICETYPE_LINEANNUNCIATOR		82
#    define SKINNY_DEVICETYPE_SOFTWAREMTPDIXIELAND	83
#    define SKINNY_DEVICETYPE_CISCOMEDIASERVER		84
#    define SKINNY_DEVICETYPE_CONFERENCEBRIDGEFLINT 	85
#    define SKINNY_DEVICETYPE_ROUTELIST 		90
#    define SKINNY_DEVICETYPE_LOADSIMULATOR 		100
#    define SKINNY_DEVICETYPE_MEDIA_TERM_POINT 		110
#    define SKINNY_DEVICETYPE_MEDIA_TERM_POINTYOKO	111
#    define SKINNY_DEVICETYPE_MEDIA_TERM_POINTDIXIELAND 112
#    define SKINNY_DEVICETYPE_MEDIA_TERM_POINTSUMMIT	113
#    define SKINNY_DEVICETYPE_MGCPSTATION		120
#    define SKINNY_DEVICETYPE_MGCPTRUNK 		121
#    define SKINNY_DEVICETYPE_RASPROXY			122
#    define SKINNY_DEVICETYPE_TRUNK			125
#    define SKINNY_DEVICETYPE_ANNUNCIATOR		126
#    define SKINNY_DEVICETYPE_MONITORBRIDGE		127
#    define SKINNY_DEVICETYPE_RECORDER			128
#    define SKINNY_DEVICETYPE_MONITORBRIDGEYOKO		129
#    define SKINNY_DEVICETYPE_SIPTRUNK			131
#    define SKINNY_DEVICETYPE_NOTDEFINED		255
#    define SKINNY_DEVICETYPE_CISCO7902 		30008
#    define SKINNY_DEVICETYPE_CISCO7905 		20000
#    define SKINNY_DEVICETYPE_CISCO7906			369			/*!< 7906 -FS */
#    define SKINNY_DEVICETYPE_CISCO7910			6			/*!< 7910 */
#    define SKINNY_DEVICETYPE_CISCO7911 		307
#    define SKINNY_DEVICETYPE_CISCO7912			30007
#    define SKINNY_DEVICETYPE_CISCO7920 		30002
#    define SKINNY_DEVICETYPE_CISCO7921 		365
#    define SKINNY_DEVICETYPE_CISCO7925 		484			/*!< 7925 -FS */
#    define SKINNY_DEVICETYPE_CISCO7931			348
#    define SKINNY_DEVICETYPE_CISCO7935 		9
#    define SKINNY_DEVICETYPE_CISCO7936 		30019
#    define SKINNY_DEVICETYPE_CISCO7937			431
#    define SKINNY_DEVICETYPE_CISCO7940			8			/*!< 7940 */
#    define SKINNY_DEVICETYPE_CISCO7941 		115
#    define SKINNY_DEVICETYPE_CISCO7941GE 		309
#    define SKINNY_DEVICETYPE_CISCO7942			434
#    define SKINNY_DEVICETYPE_CISCO7945			435
#    define SKINNY_DEVICETYPE_CISCO7960			7			/*!< 7960 */
#    define SKINNY_DEVICETYPE_CISCO7961 		30018
#    define SKINNY_DEVICETYPE_CISCO7961GE 		308
#    define SKINNY_DEVICETYPE_CISCO7962			404
#    define SKINNY_DEVICETYPE_CISCO7965 		436
#    define SKINNY_DEVICETYPE_CISCO7970 		30006
#    define SKINNY_DEVICETYPE_CISCO7971 		119
#    define SKINNY_DEVICETYPE_CISCO7975			437
#    define SKINNY_DEVICETYPE_CISCO7985			302
#    define SKINNY_DEVICETYPE_NOKIA_E_SERIES		275			/*!< Nokia S60 */
#    define SKINNY_DEVICETYPE_CISCO_IP_COMMUNICATOR	30016
#    define SKINNY_DEVICETYPE_GATEWAY_AN 		30027			/*!< Analog gateway */
#    define SKINNY_DEVICETYPE_GATEWAY_BRI 		30028			/*!< BRI gateway */
#    define SKINNY_DEVICETYPE_NOKIA_ICC 		376			/*!< nokia icc client V2 */
#    define SKINNY_DEVICETYPE_CISCO6901			547
#    define SKINNY_DEVICETYPE_CISCO6911			548
#    define SKINNY_DEVICETYPE_CISCO6921			495
#    define SKINNY_DEVICETYPE_CISCO6941			496
#    define SKINNY_DEVICETYPE_CISCO6945			564
#    define SKINNY_DEVICETYPE_CISCO6961			497
#    define SKINNY_DEVICETYPE_CISCO8941                 586
#    define SKINNY_DEVICETYPE_CISCO8945			585
#    define SKINNY_DEVICETYPE_CISCO8961                 540

/* SPA Devices */
#    define SKINNY_DEVICETYPE_SPA_521S			80000			/*!< SPA 521G */
#    define SKINNY_DEVICETYPE_SPA_525G                  80005			/*!< SPA 525G */
#    define SKINNY_DEVICETYPE_SPA_525G2			80009			/*!< SPA 525G2 */

/* Extension Modules */
#    define SKINNY_DEVICETYPE_CISCO7914 		124			/*!< Expansion module */
#    define SKINNY_DEVICETYPE_CISCO7915_12BUTTONS	227			/*!< 7915 12-Button Line Expansion Module */
#    define SKINNY_DEVICETYPE_CISCO7915			228			/*!< 7915 24-Button Line Expansion Module */
#    define SKINNY_DEVICETYPE_CISCO7916_12BUTTONS	229			/*!< 7916 12-Button Line Expansion Module */
#    define SKINNY_DEVICETYPE_CISCO7916			230			/*!< 7916 24-Button Line Expansion Module */


/*!
 * \brief Skinny DeviceType Structure
 */
static const struct skinny_devicetype {
	uint32_t devicetype;
	const char *const text;
} skinny_devicetypes[] = {
	/* *INDENT-OFF* */
	{SKINNY_DEVICETYPE_UNDEFINED, "Undefined: Maybe you forgot the devicetype in your config"},
	{SKINNY_DEVICETYPE_30SPPLUS, "30SP plus"},
	{SKINNY_DEVICETYPE_12SPPLUS, "12SP plus"},
	{SKINNY_DEVICETYPE_12SP, "12SP"},
	{SKINNY_DEVICETYPE_12, "12"},
	{SKINNY_DEVICETYPE_30VIP, "30 VIP"},
//      { SKINNY_DEVICETYPE_TELECASTER                  ,       "Telecaster"                            },
//      { SKINNY_DEVICETYPE_TELECASTER_MGR              ,       "Telecaster Manager"                    },
//      { SKINNY_DEVICETYPE_TELECASTER_BUS              ,       "Telecaster Bus"                        },
//      { SKINNY_DEVICETYPE_POLYCOM                     ,       "Polycom"                               },
	{SKINNY_DEVICETYPE_VGC, "VGC"},
	{SKINNY_DEVICETYPE_ATA186, "Cisco Ata 186"},
	{SKINNY_DEVICETYPE_ATA188, "Cisco Ata 188"},
	{SKINNY_DEVICETYPE_CISCO7910, "Cisco 7910"},
	{SKINNY_DEVICETYPE_CISCO7960, "Cisco 7960"},
	{SKINNY_DEVICETYPE_CISCO7940, "Cisco 7940"},
	{SKINNY_DEVICETYPE_VIRTUAL30SPPLUS, "Virtual 30SP plus"},
	{SKINNY_DEVICETYPE_PHONEAPPLICATION, "Phone Application"},
	{SKINNY_DEVICETYPE_ANALOGACCESS, "Analog Access"},
	{SKINNY_DEVICETYPE_DIGITALACCESSPRI, "Digital Access PRI"},
	{SKINNY_DEVICETYPE_DIGITALACCESST1, "Digital Access T1"},
	{SKINNY_DEVICETYPE_DIGITALACCESSTITAN2, "Digital Access Titan2"},
	{SKINNY_DEVICETYPE_ANALOGACCESSELVIS, "Analog Access Elvis"},
	{SKINNY_DEVICETYPE_DIGITALACCESSLENNON, "Digital Access Lennon"},
	{SKINNY_DEVICETYPE_CONFERENCEBRIDGE, "Conference Bridge"},
	{SKINNY_DEVICETYPE_CONFERENCEBRIDGEYOKO, "Conference Bridge Yoko"},
	{SKINNY_DEVICETYPE_CONFERENCEBRIDGEDIXIELAND, "Conference Bridge Dixieland"},
	{SKINNY_DEVICETYPE_CONFERENCEBRIDGESUMMIT, "Conference Bridge Summit"},
	{SKINNY_DEVICETYPE_H225, "H225"},
	{SKINNY_DEVICETYPE_H323PHONE, "H323 Phone"},
	{SKINNY_DEVICETYPE_H323TRUNK, "H323 Trunk"},
	{SKINNY_DEVICETYPE_MUSICONHOLD, "Music On Hold"},
	{SKINNY_DEVICETYPE_PILOT, "Pilot"},
	{SKINNY_DEVICETYPE_TAPIPORT, "Tapi Port"},
	{SKINNY_DEVICETYPE_TAPIROUTEPOINT, "Tapi Route Point"},
	{SKINNY_DEVICETYPE_VOICEINBOX, "Voice In Box"},
	{SKINNY_DEVICETYPE_VOICEINBOXADMIN, "Voice Inbox Admin"},
	{SKINNY_DEVICETYPE_LINEANNUNCIATOR, "Line Annunciator"},
	{SKINNY_DEVICETYPE_ROUTELIST, "Route List"},
	{SKINNY_DEVICETYPE_LOADSIMULATOR, "Load Simulator"},
	{SKINNY_DEVICETYPE_MEDIA_TERM_POINT, "Media Termination Point"},
	{SKINNY_DEVICETYPE_MEDIA_TERM_POINTYOKO, "Media Termination Point Yoko"},
	{SKINNY_DEVICETYPE_MEDIA_TERM_POINTDIXIELAND, "Media Termination Point Dixieland"},
	{SKINNY_DEVICETYPE_MEDIA_TERM_POINTSUMMIT, "Media Termination Point Summit"},
	{SKINNY_DEVICETYPE_MGCPSTATION, "MGCP Station"},
	{SKINNY_DEVICETYPE_MGCPTRUNK, "MGCP Trunk"},
	{SKINNY_DEVICETYPE_RASPROXY, "RAS Proxy"},
	{SKINNY_DEVICETYPE_TRUNK, "Trunk"},
	{SKINNY_DEVICETYPE_ANNUNCIATOR, "Annuciator"},
	{SKINNY_DEVICETYPE_MONITORBRIDGE, "Monitor Bridge"},
	{SKINNY_DEVICETYPE_RECORDER, "Recorder"},
	{SKINNY_DEVICETYPE_MONITORBRIDGEYOKO, "Monitor Bridge Yoko"},
	{SKINNY_DEVICETYPE_SIPTRUNK, "Sip Trunk"},
	{SKINNY_DEVICETYPE_NOTDEFINED, "Not Defined"},
	{SKINNY_DEVICETYPE_CISCO7920, "Cisco 7920"},
	{SKINNY_DEVICETYPE_CISCO7902, "Cisco 7902"},
	{SKINNY_DEVICETYPE_CISCO7905, "Cisco 7905"},
	{SKINNY_DEVICETYPE_CISCO7906, "Cisco 7906"},
	{SKINNY_DEVICETYPE_CISCO7911, "Cisco 7911"},
	{SKINNY_DEVICETYPE_CISCO7912, "Cisco 7912"},
	{SKINNY_DEVICETYPE_CISCO7931, "Cisco 7931"},
	{SKINNY_DEVICETYPE_CISCO7921, "Cisco 7921"},
	{SKINNY_DEVICETYPE_CISCO7925, "Cisco 7925"},
	{SKINNY_DEVICETYPE_CISCO7935, "Cisco 7935"},
	{SKINNY_DEVICETYPE_CISCO7936, "Cisco 7936"},
	{SKINNY_DEVICETYPE_CISCO7937, "Cisco 7937"},
	{SKINNY_DEVICETYPE_CISCO_IP_COMMUNICATOR, "Cisco IP Communicator"},
	{SKINNY_DEVICETYPE_CISCO7941, "Cisco 7941"},
	{SKINNY_DEVICETYPE_CISCO7941GE, "Cisco 7941 GE"},
	{SKINNY_DEVICETYPE_CISCO7942, "Cisco 7942"},
	{SKINNY_DEVICETYPE_CISCO7945, "Cisco 7945"},
	{SKINNY_DEVICETYPE_CISCO7961, "Cisco 7961"},
	{SKINNY_DEVICETYPE_CISCO7961GE, "Cisco 7961 GE"},
	{SKINNY_DEVICETYPE_CISCO7962, "Cisco 7962"},
	{SKINNY_DEVICETYPE_CISCO7965, "Cisco 7965"},
	{SKINNY_DEVICETYPE_CISCO7970, "Cisco 7970"},
	{SKINNY_DEVICETYPE_CISCO7971, "Cisco 7971"},
	{SKINNY_DEVICETYPE_CISCO7975, "Cisco 7975"},
	{SKINNY_DEVICETYPE_CISCO7985, "Cisco 7985"}, 
	{SKINNY_DEVICETYPE_NOKIA_ICC, "Nokia ICC client"},
	
	{SKINNY_DEVICETYPE_CISCO6901, "Cisco 6901"},
	{SKINNY_DEVICETYPE_CISCO6911, "Cisco 6911"},
	{SKINNY_DEVICETYPE_CISCO6921, "Cisco 6921"},
	{SKINNY_DEVICETYPE_CISCO6941, "Cisco 6941"},
	{SKINNY_DEVICETYPE_CISCO6945, "Cisco 6945"},
	{SKINNY_DEVICETYPE_CISCO6961, "Cisco 6961"},

	/* Cisco 89XX */
	{SKINNY_DEVICETYPE_CISCO8941, "Cisco 8941"},
	{SKINNY_DEVICETYPE_CISCO8945, "Cisco 8945"},
	{SKINNY_DEVICETYPE_CISCO8961, "Cisco 896"},
	
	/* SPA devices */
	{SKINNY_DEVICETYPE_SPA_521S, "Cisco SPA 521S"},
	{SKINNY_DEVICETYPE_SPA_525G, "Cisco SPA 525G"},
	{SKINNY_DEVICETYPE_SPA_525G2, "Cisco SPA 525G2"},
	
	/* *INDENT-ON* */
};

#    define SKINNY_DEVICE_RS_NONE			0
#    define SKINNY_DEVICE_RS_PROGRESS		1
#    define SKINNY_DEVICE_RS_FAILED 		2
#    define SKINNY_DEVICE_RS_OK 			3
#    define SKINNY_DEVICE_RS_TIMEOUT		4

/*!
 * \brief Skinny Device RS Structure
 */
static const struct skinny_device_registrationstate {
	uint8_t device_registrationstate;
	const char *const text;
} skinny_device_registrationstates[] = {
	/* *INDENT-OFF* */
	{SKINNY_DEVICE_RS_NONE, "None"},
	{SKINNY_DEVICE_RS_PROGRESS, "Progress"},
	{SKINNY_DEVICE_RS_FAILED, "Failed"},
	{SKINNY_DEVICE_RS_OK, "OK"}, 
	{SKINNY_DEVICE_RS_TIMEOUT, "Time Out"},
	/* *INDENT-ON* */
};

/*!
 * \brief SCCP Device State (ENUM)
 */
typedef enum {
	SCCP_DEVICESTATE_ONHOOK = 0,
	SCCP_DEVICESTATE_OFFHOOK = 1,
	SCCP_DEVICESTATE_UNAVAILABLE = 2,
	SCCP_DEVICESTATE_DND = 3,
	SCCP_DEVICESTATE_FWDALL = 4
} sccp_devicestate_t;								/*!< Internal Chan_SCCP Device State */

/*!
 * \brief Skinny Device State Structure
 */
static const struct skinny_device_state {
	sccp_devicestate_t device_state;
	const char *const text;
} skinny_device_states[] = {
	/* *INDENT-OFF* */
	{SCCP_DEVICESTATE_ONHOOK, "On Hook"},
	{SCCP_DEVICESTATE_OFFHOOK, "Off Hook"},
	{SCCP_DEVICESTATE_UNAVAILABLE, "Unavailable"},
	{SCCP_DEVICESTATE_DND, "Do Not Disturb"}, 
	{SCCP_DEVICESTATE_FWDALL, "Forward All"},
	/* *INDENT-ON* */
};

/* stimulus */
#    define SKINNY_STIMULUS_LASTNUMBERREDIAL	1
#    define SKINNY_STIMULUS_SPEEDDIAL		2
#    define SKINNY_STIMULUS_HOLD			3
#    define SKINNY_STIMULUS_TRANSFER		4
#    define SKINNY_STIMULUS_FORWARDALL		5
#    define SKINNY_STIMULUS_FORWARDBUSY 		6
#    define SKINNY_STIMULUS_FORWARDNOANSWER 	7
#    define SKINNY_STIMULUS_DISPLAY 		8
#    define SKINNY_STIMULUS_LINE			9
#    define SKINNY_STIMULUS_T120CHAT		0xA
#    define SKINNY_STIMULUS_T120WHITEBOARD		0xB
#    define SKINNY_STIMULUS_T120APPLICATIONSHARING	0xC
#    define SKINNY_STIMULUS_T120FILETRANSFER	0xD
#    define SKINNY_STIMULUS_VIDEO			0xE
#    define SKINNY_STIMULUS_VOICEMAIL		0xF
#    define SKINNY_STIMULUS_AUTOANSWERRELEASE	0x10
#    define SKINNY_STIMULUS_AUTOANSWER		0x11
#    define SKINNY_STIMULUS_SELECT			0x12
#    define SKINNY_STIMULUS_DEV_FEATURE			0x13
#    define SKINNY_STIMULUS_SERVICEURL		0x14
#    define SKINNY_STIMULUS_BLF                     0x15
#    define SKINNY_STIMULUS_MALICIOUSCALL		0x1B
#    define SKINNY_STIMULUS_GENERICAPPB1		0x21
#    define SKINNY_STIMULUS_GENERICAPPB2		0x22
#    define SKINNY_STIMULUS_GENERICAPPB3		0x23
#    define SKINNY_STIMULUS_GENERICAPPB4		0x24
#    define SKINNY_STIMULUS_GENERICAPPB5		0x25
#    define SKINNY_STIMULUS_MEETMECONFERENCE	0x7b
#    define SKINNY_STIMULUS_CONFERENCE		0x7d
#    define SKINNY_STIMULUS_CALLPARK		0x7e
#    define SKINNY_STIMULUS_CALLPICKUP		0x7f
#    define SKINNY_STIMULUS_GROUPCALLPICKUP 	0x80

/*!
 * \brief Skinny Stimulus Structure
 */
static const struct skinny_stimulus {
	uint8_t stimulus;
	const char *const text;
} skinny_stimuly[] = {
	/* *INDENT-OFF* */
	{SKINNY_STIMULUS_LASTNUMBERREDIAL, "Last Number Redial"},
	{SKINNY_STIMULUS_SPEEDDIAL, "SpeedDial"},
	{SKINNY_STIMULUS_HOLD, "Hold"},
	{SKINNY_STIMULUS_TRANSFER, "Transfer"},
	{SKINNY_STIMULUS_FORWARDALL, "Forward All"},
	{SKINNY_STIMULUS_FORWARDBUSY, "Forward Busy"},
	{SKINNY_STIMULUS_FORWARDNOANSWER, "Forward No Answer"},
	{SKINNY_STIMULUS_DISPLAY, "Display"},
	{SKINNY_STIMULUS_LINE, "Line"},
	{SKINNY_STIMULUS_T120CHAT, "T120 Chat"},
	{SKINNY_STIMULUS_T120WHITEBOARD, "T120 Whiteboard"},
	{SKINNY_STIMULUS_T120APPLICATIONSHARING, "T120 Application Sharing"},
	{SKINNY_STIMULUS_T120FILETRANSFER, "T120 File Transfer"},
	{SKINNY_STIMULUS_VIDEO, "Video"},
	{SKINNY_STIMULUS_VOICEMAIL, "VoiceMail"},
	{SKINNY_STIMULUS_AUTOANSWERRELEASE, "Auto Answer Release"},
	{SKINNY_STIMULUS_AUTOANSWER, "Auto Answer"},
	{SKINNY_STIMULUS_SELECT, "Select"},
	{SKINNY_STIMULUS_DEV_FEATURE, "DevFeature"},
	{SKINNY_STIMULUS_SERVICEURL, "ServiceURL"},
	{SKINNY_STIMULUS_BLF, "Busy Lamp Field"},
	{SKINNY_STIMULUS_MALICIOUSCALL, "Malicious Call"},
	{SKINNY_STIMULUS_GENERICAPPB1, "Generic App B1"},
	{SKINNY_STIMULUS_GENERICAPPB2, "Generic App B2"},
	{SKINNY_STIMULUS_GENERICAPPB3, "Generic App B3"},
	{SKINNY_STIMULUS_GENERICAPPB4, "Generic App B4"},
	{SKINNY_STIMULUS_GENERICAPPB5, "Generic App B5"},
	{SKINNY_STIMULUS_MEETMECONFERENCE, "Meet Me Conference"},
	{SKINNY_STIMULUS_CONFERENCE, "Conference"},
	{SKINNY_STIMULUS_CALLPARK, "Call Park"},
	{SKINNY_STIMULUS_CALLPICKUP, "Call Pickup"}, 
	{SKINNY_STIMULUS_GROUPCALLPICKUP, "Group Call Pickup"},
	/* *INDENT-ON* */
};

#    define SKINNY_BUTTONTYPE_UNUSED			0x00
#    define SKINNY_BUTTONTYPE_LASTNUMBERREDIAL		0x01
#    define SKINNY_BUTTONTYPE_SPEEDDIAL 		0x02
#    define SKINNY_BUTTONTYPE_HOLD			0x03
#    define SKINNY_BUTTONTYPE_TRANSFER			0x04
#    define SKINNY_BUTTONTYPE_FORWARDALL		0x05
#    define SKINNY_BUTTONTYPE_FORWARDBUSY		0x06
#    define SKINNY_BUTTONTYPE_FORWARDNOANSWER		0x07
#    define SKINNY_BUTTONTYPE_DISPLAY			0x08
#    define SKINNY_BUTTONTYPE_LINE			0x09
#    define SKINNY_BUTTONTYPE_T120CHAT			0x0a
#    define SKINNY_BUTTONTYPE_T120WHITEBOARD		0x0b
#    define SKINNY_BUTTONTYPE_T120APPLICATIONSHARING	0x0c
#    define SKINNY_BUTTONTYPE_T120FILETRANSFER		0x0d
#    define SKINNY_BUTTONTYPE_VIDEO 			0x0e
#    define SKINNY_BUTTONTYPE_VOICEMAIL 		0x0f
#    define SKINNY_BUTTONTYPE_ANSWERRELEASE 		0x10
#    define SKINNY_BUTTONTYPE_AUTOANSWER		0x11
#    define SKINNY_BUTTONTYPE_FEATURE        		0x13
#    define SKINNY_BUTTONTYPE_SERVICEURL		0x14
#    define SKINNY_BUTTONTYPE_BLFSPEEDDIAL		0x15
#    define SKINNY_BUTTONTYPE_GENERICAPPB1		0x21
#    define SKINNY_BUTTONTYPE_GENERICAPPB2		0x22
#    define SKINNY_BUTTONTYPE_GENERICAPPB3		0x23
#    define SKINNY_BUTTONTYPE_GENERICAPPB4		0x24
#    define SKINNY_BUTTONTYPE_GENERICAPPB5		0x25
#    define SKINNY_BUTTONTYPE_MULTIBLINKFEATURE		0x26

#    define SKINNY_BUTTONTYPE_MEETMECONFERENCE		0x7B
#    define SKINNY_BUTTONTYPE_CONFERENCE		0x7D
#    define SKINNY_BUTTONTYPE_CALLPARK			0x7E
#    define SKINNY_BUTTONTYPE_CALLPICKUP		0x7F
#    define SKINNY_BUTTONTYPE_GROUPCALLPICKUP		0x80

#    define SKINNY_BUTTONTYPE_MOBILITY			0x81
#    define SKINNY_BUTTONTYPE_TEST6			0x82
#    define SKINNY_BUTTONTYPE_TEST8			0x85
#    define SKINNY_BUTTONTYPE_TEST9			0x86
#    define SKINNY_BUTTONTYPE_TESTA			0x87
#    define SKINNY_BUTTONTYPE_TESTB			0x88
#    define SKINNY_BUTTONTYPE_TESTC			0x89
#    define SKINNY_BUTTONTYPE_TESTD			0x8A
#    define SKINNY_BUTTONTYPE_TEST7			0x8B

#    define SKINNY_BUTTONTYPE_TESTE			0xC0
#    define SKINNY_BUTTONTYPE_TESTF			0xC1
#    define SKINNY_BUTTONTYPE_MESSAGES			0xC2
#    define SKINNY_BUTTONTYPE_DIRECTORY			0xC3
#    define SKINNY_BUTTONTYPE_TESTI			0xC4
#    define SKINNY_BUTTONTYPE_APPLICATION		0xC5
#    define SKINNY_BUTTONTYPE_HEADSET			0xC6

#    define SKINNY_BUTTONTYPE_KEYPAD			0xF0
#    define SKINNY_BUTTONTYPE_AEC			0xFd
#    define SKINNY_BUTTONTYPE_UNDEFINED 		0xFF

/* this is not a skinny button type. Used for speeddial/line buttons */
#    define SCCP_BUTTONTYPE_MULTI			0xF1
#    define SCCP_BUTTONTYPE_LINE			0xF2
#    define SCCP_BUTTONTYPE_SPEEDDIAL			0xF3
#    define SCCP_BUTTONTYPE_HINT			0xF4

/*!
 * \brief Skinny ButtonType Structure
 */
static const struct skinny_buttontype {
	uint8_t buttontype;
	const char *const text;
} skinny_buttontypes[] = {
	/* *INDENT-OFF* */
	{SKINNY_BUTTONTYPE_UNUSED, "Unused"},
	{SKINNY_BUTTONTYPE_LASTNUMBERREDIAL, "Last Number Redial"},
	{SKINNY_BUTTONTYPE_SPEEDDIAL, "SpeedDial"},
	{SKINNY_BUTTONTYPE_HOLD, "Hold"},
	{SKINNY_BUTTONTYPE_TRANSFER, "Transfer"},
	{SKINNY_BUTTONTYPE_FORWARDALL, "Forward All"},
	{SKINNY_BUTTONTYPE_FORWARDBUSY, "Forward Busy"},
	{SKINNY_BUTTONTYPE_FORWARDNOANSWER, "Forward No Answer"},
	{SKINNY_BUTTONTYPE_DISPLAY, "Display"},
	{SKINNY_BUTTONTYPE_LINE, "Line"},
	{SKINNY_BUTTONTYPE_T120CHAT, "T120 Chat"},
	{SKINNY_BUTTONTYPE_T120WHITEBOARD, "T120 Whiteboard"},
	{SKINNY_BUTTONTYPE_T120APPLICATIONSHARING, "T120 Application Sharing"},
	{SKINNY_BUTTONTYPE_T120FILETRANSFER, "T120 File Transfer"},
	{SKINNY_BUTTONTYPE_VIDEO, "Video"},
	{SKINNY_BUTTONTYPE_VOICEMAIL, "Voicemail"},
	{SKINNY_BUTTONTYPE_ANSWERRELEASE, "Answer Release"},
	{SKINNY_BUTTONTYPE_AUTOANSWER, "Auto Answer"},
	{SKINNY_BUTTONTYPE_GENERICAPPB1, "Generic App B1"},
	{SKINNY_BUTTONTYPE_GENERICAPPB2, "Generic App B2"},
	{SKINNY_BUTTONTYPE_GENERICAPPB3, "Generic App B3"},
	{SKINNY_BUTTONTYPE_GENERICAPPB4, "Generic App B4"},
	{SKINNY_BUTTONTYPE_GENERICAPPB5, "Generic App B5"},
	{SKINNY_BUTTONTYPE_MEETMECONFERENCE, "Meet Me Conference"},
	{SKINNY_BUTTONTYPE_CONFERENCE, "Conference"},
	{SKINNY_BUTTONTYPE_CALLPARK, "Call Park"},
	{SKINNY_BUTTONTYPE_CALLPICKUP, "Call Pickup"},
	{SKINNY_BUTTONTYPE_GROUPCALLPICKUP, "Group Call Pickup"},
	{SKINNY_BUTTONTYPE_KEYPAD, "Keypad"},
	{SKINNY_BUTTONTYPE_AEC, "AEC"}, 
	{SKINNY_BUTTONTYPE_UNDEFINED, "Undefined"},
	/* *INDENT-ON* */
};

/* unregister */
#    define SKINNY_UNREGISTERSTATUS_OK		0
#    define SKINNY_UNREGISTERSTATUS_ERROR		1
#    define SKINNY_UNREGISTERSTATUS_NAK 		2			/*!< we have an active channel */

/*!
 * \brief Skinny Lamp Mode (ENUM) 
 */
typedef enum {
	SKINNY_LAMP_OFF = 1,							/*!< Lamp Off, 0% Duty */
	SKINNY_LAMP_ON = 2,							/*!< Lamp On, 100% Duty */
	SKINNY_LAMP_WINK = 3,							/*!< Lamp slow blink, ~90% Duty */
	SKINNY_LAMP_FLASH = 4,							/*!< Lamp very fast blink, ~70% Duty */
	SKINNY_LAMP_BLINK = 5							/*!< Lamp slow blink, ~50% Duty */
} sccp_lampMode_t;

/*!
 * \brief Skinny Lamp Structure
 */
static const struct skinny_lampmode {
	sccp_lampMode_t lampmode;
	const char *const text;
} skinny_lampmodes[] = {
	/* *INDENT-OFF* */
	{SKINNY_LAMP_OFF, "Lamp Off"},
	{SKINNY_LAMP_ON, "Lamp On"},
	{SKINNY_LAMP_WINK, "Lamp Wink"},
	{SKINNY_LAMP_FLASH, "Lamp Flash"}, 
	{SKINNY_LAMP_BLINK, "Lamp Blink"},
	/* *INDENT-ON* */
};

/* media silence suppression */
#    define SKINNY_MEDIA_SILENCESUPPRESSION_OFF 	0
#    define SKINNY_MEDIA_SILENCESUPPRESSION_ON	1

/* media echo cancel */
#    define SKINNY_MEDIA_ECHOCANCELLATION_OFF	0
#    define SKINNY_MEDIA_ECHOCANCELLATION_ON	1

/* G723 bitrate */
#    define SKINNY_MEDIA_G723BRATE_NONE 		0
#    define SKINNY_MEDIA_G723BRATE_5_3		1
#    define SKINNY_MEDIA_G723BRATE_6_4		2

/* device reset and restart */
#    define SKINNY_DEVICE_RESET 			1
#    define SKINNY_DEVICE_RESTART			2

/* statistic processing */
#    define SKINNY_STATSPROCESSING_CLEAR		0
#    define SKINNY_STATSPROCESSING_DONOTCLEAR	1

/* ringer types */
#    define SKINNY_STATION_RINGOFF			1
#    define SKINNY_STATION_INSIDERING		2
#    define SKINNY_STATION_OUTSIDERING		3
#    define SKINNY_STATION_FEATURERING		4
#    define SKINNY_STATION_SILENTRING		5
#    define SKINNY_STATION_URGENTRING		6

/*!
 * \brief Skinny Station Structure
 */
static const struct skinny_station {
	uint8_t station;
	const char *const text;
} skinny_stations[] = {
	/* *INDENT-OFF* */
	{SKINNY_STATION_RINGOFF, "Ring Off"},
	{SKINNY_STATION_INSIDERING, "Inside"},
	{SKINNY_STATION_OUTSIDERING, "Outside"},
	{SKINNY_STATION_FEATURERING, "Feature"},
	{SKINNY_STATION_SILENTRING, "Silent"}, 
	{SKINNY_STATION_URGENTRING, "Urgent"},
	/* *INDENT-ON* */
};

/* speaker status */
#    define SKINNY_STATIONSPEAKER_ON		1
#    define SKINNY_STATIONSPEAKER_OFF		2

/* mic status */
#    define SKINNY_STATIONMIC_ON			1
#    define SKINNY_STATIONMIC_OFF			2

/* headset status */
#    define SKINNY_STATIONHEADSET_ON		1
#    define SKINNY_STATIONHEADSET_OFF		2

/*!
 * \brief Skinny Codecs (ENUM) 
 */
typedef enum {
	SKINNY_CODEC_NONE = 0,
	SKINNY_CODEC_NONSTANDARD = 1,
	SKINNY_CODEC_G711_ALAW_64K = 2,
	SKINNY_CODEC_G711_ALAW_56K = 3,
	SKINNY_CODEC_G711_ULAW_64K = 4,
	SKINNY_CODEC_G711_ULAW_56K = 5,
	SKINNY_CODEC_G722_64K = 6,
	SKINNY_CODEC_G722_56K = 7,
	SKINNY_CODEC_G722_48K = 8,
	SKINNY_CODEC_G723_1 = 9,
	SKINNY_CODEC_G728 = 10,
	SKINNY_CODEC_G729 = 11,
	SKINNY_CODEC_G729_A = 12,
	SKINNY_CODEC_IS11172 = 13,
	SKINNY_CODEC_IS13818 = 14,
	SKINNY_CODEC_G729_B = 15,
	SKINNY_CODEC_G729_AB = 16,
	SKINNY_CODEC_GSM_FULLRATE = 18,
	SKINNY_CODEC_GSM_HALFRATE = 19,
	SKINNY_CODEC_GSM_ENH_FULLRATE = 20,
	SKINNY_CODEC_WIDEBAND_256K = 25,
	SKINNY_CODEC_DATA_64K = 32,
	SKINNY_CODEC_DATA_56K = 33,
	SKINNY_CODEC_G722_1_32K = 40,
	SKINNY_CODEC_G722_1_24K = 41,
	SKINNY_CODEC_AAC = 42,
	SKINNY_CODEC_GSM = 80,
	SKINNY_CODEC_ACTIVEVOICE = 81,
	SKINNY_CODEC_G726_32K = 82,
	SKINNY_CODEC_G726_24K = 83,
	SKINNY_CODEC_G726_16K = 84,
	SKINNY_CODEC_G729_ANNEX_B = 85,
	SKINNY_CODEC_G729_B_LOW = 86,
	SKINNY_CODEC_ISAC = 89,
	SKINNY_CODEC_H261 = 100,
	SKINNY_CODEC_H263 = 101,
	SKINNY_CODEC_H263P = 102,
	SKINNY_CODEC_H264 = 103,
	SKINNY_CODEC_T120 = 105,
	SKINNY_CODEC_H224 = 106,
	SKINNY_CODEC_RFC2833_DYNPAYLOAD = 257
} skinny_codec_t;

/*! 
 * \brief Skinny codec types (ENUM)
 */
typedef enum {
	SKINNY_CODEC_TYPE_UNKNOWN = 0,
	SKINNY_CODEC_TYPE_AUDIO = 1,
	SKINNY_CODEC_TYPE_VIDEO = 2,
	SKINNY_CODEC_TYPE_TEXT = 3,
	SKINNY_CODEC_TYPE_DATA = 4,
	SKINNY_CODEC_TYPE_MIXED = 5,
} skinny_payload_type_t;

/*!
 * \brief Skinny Codec Structure
 */
static const struct skinny_codec {
	skinny_codec_t codec;
	const char *const shortname;						// used in sccp.conf
	const char *const name;							// used in sccp.conf
	const char *const text;							// used to display the codec name
	skinny_payload_type_t codec_type;
	const char *mimesubtype;						// rfc mime sub-type e.g. L16
	unsigned int sample_rate;
	unsigned int sound_quality;
} skinny_codecs[] = {
	/* *INDENT-OFF* */
	{SKINNY_CODEC_NONE, 		"",		"",		"No codec", 			SKINNY_CODEC_TYPE_UNKNOWN,	NULL,		0,	0},
	{SKINNY_CODEC_NONSTANDARD,	"",		"",		"Non-standard codec", 		SKINNY_CODEC_TYPE_UNKNOWN,	NULL,		0,	0},
	{SKINNY_CODEC_IS11172, 		"is11172",	"is11172",	"IS11172 AudioCap", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_IS13818, 		"is13872",      "is13872",	"IS13818 AudioCap", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_GSM_FULLRATE, 	"gsm",		"gsm/full",	"GSM Full Rate", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	2},
	{SKINNY_CODEC_GSM_HALFRATE, 	"gsm",		"gsm/half",	"GSM Half Rate", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_GSM_ENH_FULLRATE,	"gsm",		"gsm/enh",	"GSM Enhanced Full Rate",	SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	2},
	{SKINNY_CODEC_WIDEBAND_256K, 	"slin16",	"slin16",	"Wideband 256k", 		SKINNY_CODEC_TYPE_AUDIO,	"L16",		16000,	3},
	{SKINNY_CODEC_GSM, 		"gsm",		"gsm",		"GSM", 				SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_ACTIVEVOICE, 	"activevoice",	"activevoice",	"ActiveVoice", 			SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_G711_ALAW_64K, 	"alaw",		"alaw/64k",	"G.711 A-law 64k", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	2},
	{SKINNY_CODEC_G711_ALAW_56K, 	"alaw",		"alaw/56k",	"G.711 A-law 56k", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_G711_ULAW_64K, 	"ulaw",		"ulaw/64k",	"G.711 u-law 64k", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	2},
	{SKINNY_CODEC_G711_ULAW_56K, 	"ulaw",		"ulaw/56k",	"G.711 u-law 56k", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_G722_64K, 	"g722",		"g722/64k",	"G.722 64k", 			SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	3},
	{SKINNY_CODEC_G722_56K, 	"g722",		"g722/56k",	"G.722 56k", 			SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	3},
	{SKINNY_CODEC_G722_48K, 	"g722",		"g722/48k",	"G.722 48k", 			SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	2},
	{SKINNY_CODEC_G722_1_24K, 	"g722.1",	"g722.1/24k",	"G722.1 24k (Siren7)", 		SKINNY_CODEC_TYPE_AUDIO,	"G7221",	16000,	3},
	{SKINNY_CODEC_G722_1_32K, 	"g722.1",	"g722.1/32k",	"G722.1 32k (Siren14)", 	SKINNY_CODEC_TYPE_AUDIO,	"G7221", 	32000,	4},
	{SKINNY_CODEC_G723_1, 		"g723",		"g723",		"G.723.1", 			SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_G726_16K, 	"g726",		"g726/16k",	"G.726 16K", 			SKINNY_CODEC_TYPE_AUDIO, 	NULL,		0,	1},
	{SKINNY_CODEC_G726_24K,		"g726",		"g726/24k",	"G.726 24K", 			SKINNY_CODEC_TYPE_AUDIO, 	NULL,		0,	1},
	{SKINNY_CODEC_G726_32K, 	"g726",		"g726/32k",	"G.726 32K", 			SKINNY_CODEC_TYPE_AUDIO, 	NULL,		0,	1},
	{SKINNY_CODEC_G728, 		"g728",		"g728",		"G.728", 			SKINNY_CODEC_TYPE_AUDIO, 	NULL,		0,	1},
	{SKINNY_CODEC_G729, 		"g729",		"g729",		"G.729", 			SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_G729_A, 		"g729",		"g729a",	"G.729 Annex A", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_G729_B_LOW, 	"g729",		"g729b/low",	"G.729B Low Complexity",	SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_G729_B, 		"g729",		"g729b",	"G.729 Annex B", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_G729_AB, 		"g729",		"g729ab",	"G.729 Annex A + B", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_G729_ANNEX_B, 	"g729",		"g729/annex/b",	"G.729 Annex B", 		SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_ISAC, 		"isac",		"isac",		"iSAC", 			SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_H224, 		"h224",		"h224",		"H.224", 			SKINNY_CODEC_TYPE_AUDIO,	NULL,		0,	1},
	{SKINNY_CODEC_H261, 		"h261",		"h261",		"H.261", 			SKINNY_CODEC_TYPE_VIDEO,	NULL,		0,	1},
	{SKINNY_CODEC_H263, 		"h263",		"h263",		"H.263", 			SKINNY_CODEC_TYPE_VIDEO,	"H263",		0,	1},
	{SKINNY_CODEC_H263P, 		"h263",		"h263p",	"Vieo H.263+", 			SKINNY_CODEC_TYPE_VIDEO,	NULL,		0,	1},
	{SKINNY_CODEC_H264, 		"h264",		"h264",		"H.264", 			SKINNY_CODEC_TYPE_VIDEO,	"H264",		0,	1},
	{SKINNY_CODEC_T120, 		"t120",		"t120",		"T.140", 			SKINNY_CODEC_TYPE_TEXT,		NULL,		0,	1},
	{SKINNY_CODEC_RFC2833_DYNPAYLOAD,"rfc2833",	"rfc2833",	"RFC 2833 Dyn Pay Load",	SKINNY_CODEC_TYPE_MIXED,	NULL,		0,	1},
	{SKINNY_CODEC_DATA_64K, 	"data",		"data/64k",	"Data 64k", 			SKINNY_CODEC_TYPE_DATA,		NULL,		0,	1},
	{SKINNY_CODEC_DATA_56K, 	"data",		"data/56k",	"Data 56k", 			SKINNY_CODEC_TYPE_DATA,		NULL,		0,	1},
	{SKINNY_CODEC_AAC, 		"aac",		"aac",		"AAC", 				SKINNY_CODEC_TYPE_DATA,		NULL,		0,	1},
	/* *INDENT-ON* */
};

/* device dtmfmode */
#    define SCCP_DTMFMODE_INBAND			0
#    define SCCP_DTMFMODE_OUTOFBAND 		1

/*!
 * \brief SCCP AutoAnswer Mode (ENUM)
 */
typedef enum {
	SCCP_AUTOANSWER_NONE = 0,						/*!< 1 way audio - MIC OFF */
	SCCP_AUTOANSWER_1W = 1,							/*!< 1 way audio - MIC OFF */
	SCCP_AUTOANSWER_2W = 2,							/*!< 2 way audio - MIC ON */
} sccp_autoanswer_type_t;

#    define SCCP_DNDMODE_OFF			0				/*!< dnd not permitted on the device */
#    define SCCP_DNDMODE_REJECT			1				/*!< busy signal */
#    define SCCP_DNDMODE_SILENT			2				/*!< ringing state with no ringer tone */
#    define SCCP_DNDMODE_USERDEFINED		3				/*!< the user defines the mode by pressing the softkey */



/*!
 * \brief Skinny Transmit or Receive flag
 */
typedef enum {
	SKINNY_STATION_RECEIVE		= (1<<0),
	SKINNY_STATION_TRANSMIT		= (1<<1),
} skinny_transmitOrReceive_t;

/*!
 * \brief SCCP DNDMode Structure
 */
static const struct skinny_transmitOrReceiveModes {
	skinny_transmitOrReceive_t 	mode;
	const char *const 		text;
} skinny_transmitOrReceiveModes[] = {
	/* *INDENT-OFF* */
	{SKINNY_STATION_RECEIVE, 				"Receive only"},
	{SKINNY_STATION_TRANSMIT, 				"Transmit only"},
	{SKINNY_STATION_TRANSMIT | SKINNY_STATION_RECEIVE, 	"Transmit and Receive"}, 
	/* *INDENT-ON* */
};


/*!
 * \brief SCCP DNDMode Structure
 */
static const struct sccp_dndmode {
	uint8_t dndmode;
	const char *const text;
} sccp_dndmodes[] = {
	/* *INDENT-OFF* */
	{SCCP_DNDMODE_OFF, "Off"},
	{SCCP_DNDMODE_REJECT, "Reject"},
	{SCCP_DNDMODE_SILENT, "Silent"}, 
	{SCCP_DNDMODE_USERDEFINED, "User Defined"},
	/* *INDENT-ON* */
};

#    define SCCP_BLINDTRANSFER_RING			0			/*!< default */
#    define SCCP_BLINDTRANSFER_MOH			1			/*!< music on hold */

#    define MAX_CUSTOM_PICTURES				6
#    define MAX_LAYOUT_WITH_SAME_SERVICE		5
#    define MAX_SERVICE_TYPE				4
#    define SKINNY_MAX_CAPABILITIES       		18			/*!< max capabilities allowed in Cap response message */
#    define SKINNY_MAX_VIDEO_CAPABILITIES		10
#    define SKINNY_MAX_DATA_CAPABILITIES   		5
#    define MAX_LEVEL_PREFERENCE		 	4

/*!
 * \brief Button List Structure
*/
typedef struct {
	uint8_t instance;							/*!< Button Instance */
	uint8_t type;								/*!< Button Type */
	void *ptr;								/*!< Pointer to the next Button */
} btnlist;									/*!< Button List Structure */

/*!
 * \brief Button Modes Structure
 */
typedef struct {
	const char *type;							/*!< Button Type */
	int buttonCount;							/*!< Button Count */
	const btnlist *buttons;							/*!< Button List */
} button_modes;									/*!< Button Modes Structure */

/*!
 * \brief SCCP Message Types Enum
 */
typedef enum {
	/* Client -> Server */
	KeepAliveMessage = 0x0000,
	RegisterMessage = 0x0001,
	IpPortMessage = 0x0002,
	KeypadButtonMessage = 0x0003,
	EnblocCallMessage = 0x0004,
	StimulusMessage = 0x0005,
	OffHookMessage = 0x0006,
	OnHookMessage = 0x0007,
	HookFlashMessage = 0x0008,
	ForwardStatReqMessage = 0x0009,
	SpeedDialStatReqMessage = 0x000A,
	LineStatReqMessage = 0x000B,
	ConfigStatReqMessage = 0x000C,
	TimeDateReqMessage = 0x000D,
	ButtonTemplateReqMessage = 0x000E,
	VersionReqMessage = 0x000F,
	CapabilitiesResMessage = 0x0010,
	MediaPortListMessage = 0x0011,
	ServerReqMessage = 0x0012,
	AlarmMessage = 0x0020,
	MulticastMediaReceptionAck = 0x0021,
	OpenReceiveChannelAck = 0x0022,
	ConnectionStatisticsRes = 0x0023,
	OffHookWithCgpnMessage = 0x0024,
	SoftKeySetReqMessage = 0x0025,
	SoftKeyEventMessage = 0x0026,
	UnregisterMessage = 0x0027,
	SoftKeyTemplateReqMessage = 0x0028,
	RegisterTokenReq = 0x0029,
	HeadsetStatusMessage = 0x002B,
	MediaResourceNotification = 0x002C,
	RegisterAvailableLinesMessage = 0x002D,
	DeviceToUserDataMessage = 0x002E,
	DeviceToUserDataResponseMessage = 0x002F,
	UpdateCapabilitiesMessage = 0x0030,
	OpenMultiMediaReceiveChannelAckMessage = 0x0031,
	ClearConferenceMessage = 0x0032,
	ServiceURLStatReqMessage = 0x0033,
	FeatureStatReqMessage = 0x0034,
	CreateConferenceResMessage = 0x0035,
	DeleteConferenceResMessage = 0x0036,
	ModifyConferenceResMessage = 0x0037,
	AddParticipantResMessage = 0x0038,
	AuditConferenceResMessage = 0x0039,
	AuditParticipantResMessage = 0x0040,
	DeviceToUserDataVersion1Message = 0x0041,
	DeviceToUserDataResponseVersion1Message = 0x0042,

	/* This are from protocol V 11 CCM7 */
	DialedPhoneBookMessage = 0x0048,
	AccessoryStatusMessage = 0x0049,
	Unknown_0x004A_Message = 0x004A,

	/* Server -> Client */
	RegisterAckMessage = 0x0081,
	StartToneMessage = 0x0082,
	StopToneMessage = 0x0083,
	// ??
	SetRingerMessage = 0x0085,
	SetLampMessage = 0x0086,
	SetHkFDetectMessage = 0x0087,
	SetSpeakerModeMessage = 0x0088,
	SetMicroModeMessage = 0x0089,
	StartMediaTransmission = 0x008A,
	StopMediaTransmission = 0x008B,
	StartMediaReception = 0x008C,
	StopMediaReception = 0x008D,
	// ?
	CallInfoMessage = 0x008F,

	ForwardStatMessage = 0x0090,
	ForwardStatMessageV19 = 0x0090,
	SpeedDialStatMessage = 0x0091,
	LineStatMessage = 0x0092,
	ConfigStatMessage = 0x0093,
	DefineTimeDate = 0x0094,
	StartSessionTransmission = 0x0095,
	StopSessionTransmission = 0x0096,
	ButtonTemplateMessage = 0x0097,
	ButtonTemplateMessageSingle = 0x0097,
	VersionMessage = 0x0098,
	DisplayTextMessage = 0x0099,
	ClearDisplay = 0x009A,
	CapabilitiesReqMessage = 0x009B,
	EnunciatorCommandMessage = 0x009C,
	RegisterRejectMessage = 0x009D,
	ServerResMessage = 0x009E,
	Reset = 0x009F,

	KeepAliveAckMessage = 0x0100,
	StartMulticastMediaReception = 0x0101,
	StartMulticastMediaTransmission = 0x0102,
	StopMulticastMediaReception = 0x0103,
	StopMulticastMediaTransmission = 0x0104,
	OpenReceiveChannel = 0x0105,
	CloseReceiveChannel = 0x0106,
	ConnectionStatisticsReq = 0x0107,
	ConnectionStatisticsReq_V19 = 0x0107,
	SoftKeyTemplateResMessage = 0x0108,
	SoftKeySetResMessage = 0x0109,

	SelectSoftKeysMessage = 0x0110,
	CallStateMessage = 0x0111,
	DisplayPromptStatusMessage = 0x0112,
	ClearPromptStatusMessage = 0x0113,
	DisplayNotifyMessage = 0x0114,
	ClearNotifyMessage = 0x0115,
	ActivateCallPlaneMessage = 0x0116,
	DeactivateCallPlaneMessage = 0x0117,
	UnregisterAckMessage = 0x0118,
	BackSpaceReqMessage = 0x0119,
	RegisterTokenAck = 0x011A,
	RegisterTokenReject = 0x011B,
	StartMediaFailureDetection = 0x011C,
	DialedNumberMessage = 0x011D,
	DialedNumberMessageV19 = 0x011D,
	UserToDeviceDataMessage = 0x011E,
	FeatureStatMessage = 0x011F,
	DisplayPriNotifyMessage = 0x0120,
	ClearPriNotifyMessage = 0x0121,
	StartAnnouncementMessage = 0x0122,
	StopAnnouncementMessage = 0x0123,
	AnnouncementFinishMessage = 0x0124,
	NotifyDtmfToneMessage = 0x0127,
	SendDtmfToneMessage = 0x0128,
	SubscribeDtmfPayloadReqMessage = 0x0129,
	SubscribeDtmfPayloadResMessage = 0x012A,
	SubscribeDtmfPayloadErrMessage = 0x012B,
	UnSubscribeDtmfPayloadReqMessage = 0x012C,
	UnSubscribeDtmfPayloadResMessage = 0x012D,
	UnSubscribeDtmfPayloadErrMessage = 0x012E,
	ServiceURLStatMessage = 0x012F,
	CallSelectStatMessage = 0x0130,
	OpenMultiMediaChannelMessage = 0x0131,
	StartMultiMediaTransmission = 0x0132,
	StopMultiMediaTransmission = 0x0133,
	MiscellaneousCommandMessage = 0x0134,
	FlowControlCommandMessage = 0x0135,
	CloseMultiMediaReceiveChannel = 0x0136,
	CreateConferenceReqMessage = 0x0137,
	DeleteConferenceReqMessage = 0x0138,
	ModifyConferenceReqMessage = 0x0139,
	AddParticipantReqMessage = 0x013A,
	DropParticipantReqMessage = 0x013B,
	AuditConferenceReqMessage = 0x013C,
	AuditParticipantReqMessage = 0x013D,
	UserToDeviceDataVersion1Message = 0x013F,

	/* sent by us */
	Unknown_0x0141_Message = 0x0141,
        DisplayDynamicNotifyMessage = 0x0143,
        DisplayDynamicPriNotifyMessage = 0x0144,
	DisplayDynamicPromptStatusMessage = 0x0145,
	FeatureStatDynamicMessage = 0x0146,
	LineStatDynamicMessage = 0x0147,
	ServiceURLStatDynamicMessage = 0x0148,
	SpeedDialStatDynamicMessage = 0x0149,
	CallInfoDynamicMessage = 0x014A,

	/* received from phone */
	DialedPhoneBookAckMessage = 0x0152,
	Unknown_0x0153_Message = 0x0153,
	StartMediaTransmissionAck = 0x0154,
	ExtensionDeviceCaps = 0x0159,
	XMLAlarmMessage = 0x015A,

	/* SPCP client -> server */
	SPCPRegisterTokenRequest = 0x8000,
	
	/* SPCP server -> client */
	SPCPRegisterTokenAck = 0x8100,
	SPCPRegisterTokenReject = 0x8101,                                   
} sccp_message_t;								/*!< SCCP Message Types Enum */

/*!
 * \brief SCCP Message Type Structure
 */
static const struct sccp_messagetype {
	sccp_message_t type;
	const char *const text;
} sccp_messagetypes[] = {
	/* *INDENT-OFF* */
	{KeepAliveMessage, "Keep Alive Message"},
	{RegisterMessage, "Register Message"},
	{IpPortMessage, "Ip-Port Message"},
	{KeypadButtonMessage, "Keypad Button Message"},
	{EnblocCallMessage, "Enbloc Call Message"},
	{StimulusMessage, "Stimulus Message"},
	{OffHookMessage, "Off-Hook Message"},
	{OnHookMessage, "On-Hook Message"},
	{HookFlashMessage, "Hook-Flash Message"},
	{ForwardStatReqMessage, "Forward State Request"},
	{SpeedDialStatReqMessage, "Speed-Dial State Request"},
	{LineStatReqMessage, "Line State Request"},
	{ConfigStatReqMessage, "Config State Request"},
	{TimeDateReqMessage, "Time Date Request"},
	{ButtonTemplateReqMessage, "Button Template Request"},
	{VersionReqMessage, "Version Request"},
	{CapabilitiesResMessage, "Capabilities Response Message"},
	{MediaPortListMessage, "Media Port List Message"},
	{ServerReqMessage, "Server Request"},
	{AlarmMessage, "Alarm Message"},
	{MulticastMediaReceptionAck, "Multicast Media Reception Acknowledge"},
	{OpenReceiveChannelAck, "Open Receive Channel Acknowledge"},
	{ConnectionStatisticsRes, "Connection Statistics Response"},
	{OffHookWithCgpnMessage, "Off-Hook With Cgpn Message"},
	{SoftKeySetReqMessage, "SoftKey Set Request"},
	{SoftKeyEventMessage, "SoftKey Event Message"},
	{UnregisterMessage, "Unregister Message"},
	{SoftKeyTemplateReqMessage, "SoftKey Template Request"},
	{RegisterTokenReq, "Register Token Request"},
	{HeadsetStatusMessage, "Headset Status Message"},
	{MediaResourceNotification, "Media Resource Notification"},
	{RegisterAvailableLinesMessage, "Register Available Lines Message"},
	{DeviceToUserDataMessage, "Device To User Data Message"},
	{DeviceToUserDataResponseMessage, "Device To User Data Response"},
	{UpdateCapabilitiesMessage, "Update Capabilities Message"},
	{OpenMultiMediaReceiveChannelAckMessage, "Open MultiMedia Receive Channel Acknowledge"},
	{ClearConferenceMessage, "Clear Conference Message"},
	{ServiceURLStatReqMessage, "Service URL State Request"},
	{FeatureStatReqMessage, "Feature State Request"},
	{CreateConferenceResMessage, "Create Conference Response"},
	{DeleteConferenceResMessage, "Delete Conference Response"},
	{ModifyConferenceResMessage, "Modify Conference Response"},
	{AddParticipantResMessage, "Add Participant Response"},
	{AuditConferenceResMessage, "Audit Conference Response"},
	{AuditParticipantResMessage, "Audit Participant Response"},
	{DeviceToUserDataVersion1Message, "Device To User Data Version1 Message"},
	{DeviceToUserDataResponseVersion1Message, "Device To User Data Version1 Response"},
	{DialedPhoneBookMessage, "Dialed PhoneBook Message"},
	{AccessoryStatusMessage, "Accessory Status Message"},
	{Unknown_0x004A_Message, "Undefined 0x004A Message"},
	{RegisterAckMessage, "Register Acknowledge"},
	{StartToneMessage, "Start Tone Message"},
	{StopToneMessage, "Stop Tone Message"},
	{SetRingerMessage, "Set Ringer Message"},
	{SetLampMessage, "Set Lamp Message"},
	{SetHkFDetectMessage, "Set HkF Detect Message"},
	{SetSpeakerModeMessage, "Set Speaker Mode Message"},
	{SetMicroModeMessage, "Set Micro Mode Message"},
	{StartMediaTransmission, "Start Media Transmission"},
	{StopMediaTransmission, "Stop Media Transmission"},
	{StartMediaReception, "Start Media Reception"},
	{StopMediaReception, "Stop Media Reception"},
	{CallInfoMessage, "Call Information Message"},
	{ForwardStatMessage, "Forward State Message"},
	{SpeedDialStatMessage, "SpeedDial State Message"},
	{LineStatMessage, "Line State Message"},
	{ConfigStatMessage, "Config State Message"},
	{DefineTimeDate, "Define Time Date"},
	{StartSessionTransmission, "Start Session Transmission"},
	{StopSessionTransmission, "Stop Session Transmission"},
	{ButtonTemplateMessage, "Button Template Message"},
	{ButtonTemplateMessageSingle, "Button Template Message Single"},
	{VersionMessage, "Version Message"},
	{DisplayTextMessage, "Display Text Message"},
	{ClearDisplay, "Clear Display"},
	{CapabilitiesReqMessage, "Capabilities Request"},
	{EnunciatorCommandMessage, "Enunciator Command Message"},
	{RegisterRejectMessage, "Register Reject Message"},
	{ServerResMessage, "Server Response"},
	{Reset, "Reset"},
	{KeepAliveAckMessage, "Keep Alive Acknowledge"},
	{StartMulticastMediaReception, "Start MulticastMedia Reception"},
	{StartMulticastMediaTransmission, "Start MulticastMedia Transmission"},
	{StopMulticastMediaReception, "Stop MulticastMedia Reception"},
	{StopMulticastMediaTransmission, "Stop MulticastMedia Transmission"},
	{OpenReceiveChannel, "Open Receive Channel"},
	{CloseReceiveChannel, "Close Receive Channel"},
	{ConnectionStatisticsReq, "Connection Statistics Request"},
	{SoftKeyTemplateResMessage, "SoftKey Template Response"},
	{SoftKeySetResMessage, "SoftKey Set Response"},
	{SelectSoftKeysMessage, "Select SoftKeys Message"},
	{CallStateMessage, "Call State Message"},
	{DisplayPromptStatusMessage, "Display Prompt Status Message"},
	{ClearPromptStatusMessage, "Clear Prompt Status Message"},
	{DisplayNotifyMessage, "Display Notify Message"},
	{ClearNotifyMessage, "Clear Notify Message"},
	{ActivateCallPlaneMessage, "Activate Call Plane Message"},
	{DeactivateCallPlaneMessage, "Deactivate Call Plane Message"},
	{UnregisterAckMessage, "Unregister Acknowledge"},
	{BackSpaceReqMessage, "Back Space Request"},
	{RegisterTokenAck, "Register Token Acknowledge"},
	{RegisterTokenReject, "Register Token Reject"},
	{StartMediaFailureDetection, "Start Media Failure Detection"},
	{DialedNumberMessage, "Dialed Number Message"},
	{UserToDeviceDataMessage, "User To Device Data Message"},
	{FeatureStatMessage, "Feature State Message"},
	{DisplayPriNotifyMessage, "Display Pri Notify Message"},
	{ClearPriNotifyMessage, "Clear Pri Notify Message"},
	{StartAnnouncementMessage, "Start Announcement Message"},
	{StopAnnouncementMessage, "Stop Announcement Message"},
	{AnnouncementFinishMessage, "Announcement Finish Message"},
	{NotifyDtmfToneMessage, "Notify DTMF Tone Message"},
	{SendDtmfToneMessage, "Send DTMF Tone Message"},
	{SubscribeDtmfPayloadReqMessage, "Subscribe DTMF Payload Request"},
	{SubscribeDtmfPayloadResMessage, "Subscribe DTMF Payload Response"},
	{SubscribeDtmfPayloadErrMessage, "Subscribe DTMF Payload Error Message"},
	{UnSubscribeDtmfPayloadReqMessage, "UnSubscribe DTMF Payload Request"},
	{UnSubscribeDtmfPayloadResMessage, "UnSubscribe DTMF Payload Response"},
	{UnSubscribeDtmfPayloadErrMessage, "UnSubscribe DTMF Payload Error Message"},
	{ServiceURLStatMessage, "ServiceURL State Message"},
	{CallSelectStatMessage, "Call Select State Message"},
	{OpenMultiMediaChannelMessage, "Open MultiMedia Channel Message"},
	{StartMultiMediaTransmission, "Start MultiMedia Transmission"},
	{StopMultiMediaTransmission, "Stop MultiMedia Transmission"},
	{MiscellaneousCommandMessage, "Miscellaneous Command Message"},
	{FlowControlCommandMessage, "Flow Control Command Message"},
	{CloseMultiMediaReceiveChannel, "Close MultiMedia Receive Channel"},
	{CreateConferenceReqMessage, "Create Conference Request"},
	{DeleteConferenceReqMessage, "Delete Conference Request"},
	{ModifyConferenceReqMessage, "Modify Conference Request"},
	{AddParticipantReqMessage, "Add Participant Request"},
	{DropParticipantReqMessage, "Drop Participant Request"},
	{AuditConferenceReqMessage, "Audit Conference Request"},
	{AuditParticipantReqMessage, "Audit Participant Request"},
	{UserToDeviceDataVersion1Message, "User To Device Data Version1 Message"},
	{DisplayDynamicNotifyMessage, "Display Dynamic Notify Message"},
	{DisplayDynamicPriNotifyMessage, "Display Dynamic Priority Notify Message"},
	{DisplayDynamicPromptStatusMessage, "Display Dynamic Prompt Status Message"},
	{FeatureStatDynamicMessage, "SpeedDial State Dynamic Message"},
	{LineStatDynamicMessage, "Line State Dynamic Message"},
	{ServiceURLStatDynamicMessage, "Service URL Stat Dynamic Messages"},
	{SpeedDialStatDynamicMessage, "SpeedDial Stat Dynamic Message"},
	{CallInfoDynamicMessage, "Call Information Dynamic Message"},
	{DialedPhoneBookAckMessage, "Dialed PhoneBook Ack Message"},
	{Unknown_0x0153_Message, "Undefined 0x0153 Message"},
	{StartMediaTransmissionAck, "Start Media Transmission Acknowledge"},
	{ExtensionDeviceCaps, "Extension Device Capabilities Message"},
	{XMLAlarmMessage, "XML-AlarmMessage"},
	{SPCPRegisterTokenRequest, "SPCP Register Token RequestCODEC"},
	{SPCPRegisterTokenAck, "SCPA RegisterMessageACK"},
	{SPCPRegisterTokenReject, "SCPA RegisterMessageReject"},
	/* *INDENT-ON* */
};

#    define SCCP_ACCESSORY_NONE			0x00				/*!< Added for compatibility with old phones -FS */
#    define SCCP_ACCESSORY_HEADSET			0x01
#    define SCCP_ACCESSORY_HANDSET			0x02
#    define SCCP_ACCESSORY_SPEAKER			0x03

/*!
 * \brief SCCP Accessory Structure
 */
static const struct sccp_accessory {
	uint8_t accessory;
	const char *const text;
} sccp_accessories[] = {
	/* *INDENT-OFF* */
	{SCCP_ACCESSORY_NONE, "None"},
	{SCCP_ACCESSORY_HEADSET, "Headset"},
	{SCCP_ACCESSORY_HANDSET, "Handset"}, 
	{SCCP_ACCESSORY_SPEAKER, "Speaker"},
	/* *INDENT-ON* */
};

#    define SCCP_ACCESSORYSTATE_NONE		0x00				/*!< Added for compatibility with old phones -FS */
#    define SCCP_ACCESSORYSTATE_OFFHOOK		0x01
#    define SCCP_ACCESSORYSTATE_ONHOOK		0x02

/*!
 * \brief SCCP Accessory State Structure
 */
static const struct sccp_accessory_state {
	uint8_t accessory_state;
	const char *const text;
} sccp_accessory_states[] = {
	/* *INDENT-OFF* */
	{SCCP_ACCESSORYSTATE_NONE, "None"},
	{SCCP_ACCESSORYSTATE_ONHOOK, "On Hook"}, 
	{SCCP_ACCESSORYSTATE_OFFHOOK, "Off Hook"},
	/* *INDENT-ON* */
};

/*!
 * \brief SCCP Extension State Structure
 */
static const struct sccp_extension_state {
	uint16_t extension_state;
	const char *const text;
} sccp_extension_states[] = {
	/* *INDENT-OFF* */
	{AST_EXTENSION_REMOVED, "Extension Removed"},
	{AST_EXTENSION_DEACTIVATED, "Extension Hint Removed"},
	{AST_EXTENSION_NOT_INUSE, "No device INUSE or BUSY"},
	{AST_EXTENSION_INUSE, "One or More devices In Use"},
	{AST_EXTENSION_BUSY, "All devices Busy"},
	{AST_EXTENSION_UNAVAILABLE, "All devices Unavailable/Unregistered"},
#    ifdef CS_AST_HAS_EXTENSION_RINGING
	{AST_EXTENSION_RINGING, "All Devices Ringing"},
	{AST_EXTENSION_INUSE | AST_EXTENSION_RINGING, "All Devices Ringing and In Use"},
#    endif
#    ifdef CS_AST_HAS_EXTENSION_ONHOLD
	{AST_EXTENSION_ONHOLD, "All Devices On Hold"},
#    endif
	/* *INDENT-ON* */
};

/*=====================================================================================================*/

/*!
 * \brief Station Identifier Structure
 */
typedef struct {
	char deviceName[StationMaxDeviceNameSize];				/*!< Device name */
	uint32_t lel_userid;							/*!< User ID */
	uint32_t lel_instance;							/*!< Instance */
} StationIdentifier;								/*!< Station Identifier Structure */

/*!
 * \brief Station Button Definition Structure
 */
typedef struct {
	uint8_t instanceNumber;							/*!< set to instance number or StationKeyPadButton value */
	uint8_t buttonDefinition;						/*!< set to one of the preceding Bt values */
} StationButtonDefinition;							/*!< Station Button Definition Structure */

/*!
 * \brief Media Payload Capability Structure
 */
typedef struct {
	skinny_codec_t lel_payloadCapability;					/*!< Payload Capability (Codec Used) */
	uint32_t lel_maxFramesPerPacket;					/*!< Maximum Number of Frames per IP Packet / Number of milliseconds of audio per RTP Packet */
	union {
		uint8_t futureUse[8];
		uint8_t lel_g723BitRate;					/*!< g723 Bit Rate (1=5.3 Kbps, 2=6.4 Kbps) */
	} PAYLOADS;								/*!< Payloads */
} MediaCapabilityStructure;							/*!< Media Payload Capability Structure */

/*!
 * \brief Station Soft Key Definition Structure
 */
typedef struct {
	char softKeyLabel[StationMaxSoftKeyLabelSize];				/*!< Soft Key Label */
	uint32_t lel_softKeyEvent;						/*!< Soft Key Event */
} StationSoftKeyDefinition;							/*!< Station Soft Key Definition Structure */

/*!
 * \brief Station Soft Key Set Definition Structure
 */
typedef struct {
	uint8_t softKeyTemplateIndex[StationMaxSoftKeyIndex];			/*!< Soft Key Template Index */
	uint16_t les_softKeyInfoIndex[StationMaxSoftKeyIndex];			/*!< Soft Key Info Index */
} StationSoftKeySetDefinition;							/*!< Station Soft Key Set Definition Structure */

/*!
 * \brief Station Identifier Structure
 */
typedef struct {
	char serverName[ServerMaxNameSize];					/*!< Server Name */
} ServerIdentifier;								/*!< Server Identifier Structure */

/*! 
 * \brief Layout Config Structure (Update Capabilities Message Struct)
 * \since 20080111
 */
typedef struct {
	uint32_t layout;							/*!< Layout \todo what is layout? */
} layoutConfig_t;								/*!< Layout Config Structure */

/*!
 * \brief Video Level Preference Structure
 */
typedef struct {
	uint32_t transmitPreference;						/*!< Transmit Preference */
	uint32_t format;							/*!< Format / Codec */
	uint32_t maxBitRate;							/*!< Maximum BitRate */
	uint32_t minBitRate;							/*!< Minimum BitRate */
	uint32_t MPI;								/*!<  */
	uint32_t serviceNumber;							/*!< Service Number */
} levelPreference_t;								/*!< Level Preference Structure */

/*!
 * \brief Service Resource Structure
 */
typedef struct {
	uint32_t layoutCount;							/*!< Layout Count */
	layoutConfig_t layout[MAX_LAYOUT_WITH_SAME_SERVICE];			/*!< Layout */
	uint32_t serviceNum;							/*!< Service Number */
	uint32_t maxStreams;							/*!< Maximum number of Streams */
	uint32_t maxConferences;						/*!< Maximum number of Conferences */
	uint32_t activeConferenceOnRegistration;				/*!< Active Conference On Registration */
} serviceResource_t;								/*!< Service Resource Structure */

/*!
 * \brief Picture Format Structure
 */
typedef struct {
	uint32_t customPictureFormatWidth;					/*!< Picture Width */
	uint32_t customPictureFormatHeight;					/*!< Picture Height */
	uint32_t customPictureFormatpixelAspectRatio;				/*!< Picture Pixel Aspect Ratio */
	uint32_t customPictureFormatpixelclockConversionCode;			/*!< Picture Pixel Conversion Code  */
	uint32_t customPictureFormatpixelclockDivisor;				/*!< Picture Pixel Divisor */
} customPictureFormat_t;							/*!< Picture Format Structure */

/*!
 * \brief Audio Capabilities Structure
 */
typedef struct {
	skinny_codec_t lel_payloadCapability;					/*!< PayLoad Capability */
	uint32_t lel_maxFramesPerPacket;					/*!< Maximum Number of Frames per IP Packet */
	uint32_t lel_unknown[2];						/*!< this are related to G.723 */
} audioCap_t;									/*!< Audio Capabilities Structure */

/*!
 * \brief Video Capabilities Structure
 */
typedef struct {
	skinny_codec_t lel_payloadCapability;					/*!< PayLoad Capability */
	uint32_t lel_transmitOreceive;						/*!< Transmit of Receive */
	uint32_t lel_levelPreferenceCount;					/*!< Level of Preference Count */

	levelPreference_t levelPreference[MAX_LEVEL_PREFERENCE];		/*!< Level Preference */

// 	uint32_t lel_codec_options[2];						/*!< Codec Options */
	
	union {
		struct {
			uint32_t unknown1;
			uint32_t unknown2;
		} h263;
		struct {
			uint32_t profile;					/*!< H264 profile */
			uint32_t level;						/*!< H264 level */
		} h264;
	} codec_options;
	

	/**
         * Codec options contains data specific for every codec
         *
         * Here is a list of known parameters per codec
        // H.261
        uint32_t		lel_temporalSpatialTradeOffCapability;
        uint32_t		lel_stillImageTransmission;

        // H.263
        uint32_t		lel_h263_capability_bitfield;
        uint32_t		lel_annexNandWFutureUse;

        // Video
        uint32_t		lel_modelNumber;
        uint32_t		lel_bandwidth;
        */
} videoCap_t;									/*!< Video Capabilities Structure */

/*!
 * \brief Data Capabilities Structure
 */
typedef struct {
	uint32_t payloadCapability;						/*!< Payload Capability */
	uint32_t transmitOrReceive;						/*!< Transmit or Receive */
	uint32_t protocolDependentData;						/*!< Protocol Dependent Data */
	uint32_t maxBitRate;							/*!< Maximum BitRate */
} dataCap_t;									/*!< Data Capabilities Structure */

/*!
 * \brief Audio Parameters Structure
 */
typedef struct {
	uint32_t millisecondPacketSize;						/*!< Packet Size per Milli Second */
	uint32_t lel_echoCancelType;						/*!< Echo Cancelation Type */
	uint32_t lel_g723BitRate;						/*!< only used with G.723 payload */
} audioParameter_t;								/*!< Audio Parameters Structure */

/*!
 * \brief Picture Format Structure
 */
typedef struct {
	uint32_t format;							/*!< Picture Format */
	uint32_t mpi;								/*!< MPI */
} pictureFormat_t;								/*!< Picture Format Structure */

/*!
 * \brief H261 Video Capability Structure
 */
typedef struct {
	uint32_t temporalSpatialTradeOffCapability;				/*!< Temporal Spatial Trade Off Capability */
	uint32_t stillImageTransmission;					/*!< Still Image Transmission */
} h261VideoCapability_t;							/*!< H261 Video Capability Structure */

/*!
 * \brief H263 Video Capability Structure
 */
typedef struct {
	uint32_t h263CapabilityBitfield;					/*!< H263 Capability Bit-Field */
	uint32_t annexNandwFutureUse;						/*!< Annex Nandw for Future Use */
} h263VideoCapability_t;							/*!< H262 Vidio Capability Structure */

/*!
 * \brief vieo Video Capability Structure
 */
typedef struct {
	uint32_t modelNumber;							/*!< Model Number */
	uint32_t bandwidth;							/*!< BandWidth */
} vieoVideoCapability_t;							/*!< vieo Video Capability Structure */

/*!
 * \brief Data Parameter Structure
 */
typedef struct {
	uint32_t protocolDependentData;						/*!< Protocol Dependent Data */
	uint32_t maxBitRate;							/*!< Maximum BitRate */
} dataParameter_t;								/*!< Data Parameter Structure */

/*! 
 * \brief Video Parameter Structure 
 * \since 20100103
 */
// typedef struct {
// 	uint32_t bitRate;							/*!< BitRate (default 384) */
// 	uint32_t pictureFormatCount;						/*!< Picture Format Count (default 0) */
// 	pictureFormat_t pictureFormat[3];					/*!< Picture Format Array */
// 	uint32_t confServiceNum;						/*!< Conf Service Number */
// 	uint32_t dummy;								/*!< dummy */
// 	h261VideoCapability_t h261VideoCapability;				/*!< H261 Video Capability */
// 	h263VideoCapability_t h263VideoCapability;				/*!< H263 Video Capability */
// 	vieoVideoCapability_t vieoVideoCapability;				/*!< vieo Video Capability */
// } videoParameter_t;								/*!< Video Parameter Structure */
/*! 
	 * \brief Video Parameter Structure 
	 * \since 20110718
	 */
	typedef struct {
	        uint32_t bitRate;                                                       /*!< BitRate (default 384) */
	        uint32_t pictureFormatCount;                                            /*!< Picture Format Count (default 0) */
	        pictureFormat_t pictureFormat[5];                                       /*!< Picture Format Array */
	        uint32_t confServiceNum;                                                /*!< Conf Service Number */
	        uint32_t profile;
	        uint32_t level;
	        uint32_t macroblockspersec;
	        uint32_t macroblocksperframe;
	        uint32_t decpicbuf;
	        uint32_t brandcpb;
	        uint16_t dummy1;
	        uint16_t dummy2;
	        uint32_t dummy3;
	        uint32_t dummy4;
	        uint32_t dummy5;
	        uint32_t dummy6;
	        uint32_t dummy7;
	        uint32_t dummy8;
	} videoParameter_t;                                                             /*!< Video Parameter Structure */




/*!
 * \brief SCCP Data Union
 */
typedef union {

	/* 0x0048 is phone call list update (comes from phone 7921
	 * phones when you press the dial button without a number.
	 * It looks like this:
	 *
	 * 00000000 - 00 00 00 00 01 00 00 00 4B 0D 00 00 34 30 30 30 ........K...4000
	 * 00000010 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000020 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000030 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000040 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000050 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000060 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000070 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000080 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000090 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 000000A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 000000B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 000000C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 000000D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 000000E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 000000F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000100 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * First DWORD is Index shifted of 4 bits (i think the 4 bits param is something)
	 * Second DWORD is line instance related to the call
	 * Third DWORD is unknown, Next there is the phone number [260 bytes]
	 */

	struct {
		uint32_t lel_NumberIndex;					/*!< Number Index (this must be shifted 4 bits right) */
		uint32_t lel_lineinstance;					/*!< Line Instance */
		uint32_t lel_unknown;						/*!< \todo Unknown */
		char phonenumber[260];						/*!< \todo I don't know if this is exact */
	} DialedPhoneBookMessage;						/*!< Dialed Phone Book Message Structure */

	struct {
		uint32_t lel_NumberIndex;					/*!< Number Index (this must be shifted 4 bits right) */
		uint32_t lel_lineinstance;					/*!< Line Instance */
		uint32_t lel_unknown;						/*!< \todo Unknown */
		uint32_t lel_unknown2;						/*!< \todo Unknown2 */
	} DialedPhoneBookAckMessage;						/*!< Dialed Phone Book Acknowledgement Structure */

	struct {
		uint32_t lel_appID;
		uint32_t lel_lineInstance;
		uint32_t lel_callReference;
		uint32_t lel_transactionID;
		uint32_t lel_dataLength;
		char data[StationMaxXMLMessage];
	} UserToDeviceDataMessage;						/*!< User to Device Message Structure */

	struct {
		uint32_t lel_appID;
		uint32_t lel_lineInstance;
		uint32_t lel_callReference;
		uint32_t lel_transactionID;
		uint32_t lel_dataLength;
		uint32_t lel_sequenceFlag;
		uint32_t lel_displayPriority;
		uint32_t lel_conferenceID;
		uint32_t lel_appInstanceID;
		uint32_t lel_routing;
		char data;							// dummy char for variable length message
	} UserToDeviceDataVersion1Message;					/*!< User to Device Version1 Message Structure */

	struct {
		uint32_t lel_appID;
		uint32_t lel_lineInstance;
		uint32_t lel_callReference;
		uint32_t lel_transactionID;
		uint32_t lel_dataLength;
		char data[StationMaxXMLMessage];
	} DeviceToUserDataMessage;						/*!< Device to User Message Structure */

	struct {
		uint32_t lel_appID;
		uint32_t lel_lineInstance;
		uint32_t lel_callReference;
		uint32_t lel_transactionID;
		uint32_t lel_dataLength;
		char data[StationMaxXMLMessage];
	} DeviceToUserDataResponseMessage;					/*!< Device to User Response Message Structure */

	struct {
		uint32_t lel_appID;
		uint32_t lel_lineInstance;
		uint32_t lel_callReference;
		uint32_t lel_transactionID;
		uint32_t lel_dataLength;
		uint32_t lel_sequenceFlag;
		uint32_t lel_displayPriority;
		uint32_t lel_conferenceID;
		uint32_t lel_appInstanceID;
		uint32_t lel_routing;
		char data[StationMaxXMLMessage];
	} DeviceToUserDataVersion1Message;					/*!< Device to User Version1 Message Structure */

	struct {
		uint32_t lel_appID;
		uint32_t lel_lineInstance;
		uint32_t lel_callReference;
		uint32_t lel_transactionID;
		uint32_t lel_dataLength;
		uint32_t lel_sequenceFlag;
		uint32_t lel_displayPriority;
		uint32_t lel_conferenceID;
		uint32_t lel_appInstanceID;
		uint32_t lel_routing;
		char data[StationMaxXMLMessage];
	} DeviceToUserDataResponseVersion1Message;

	/* AccessoryStatusMessage (0x0073):
	 * This indicates the phone headset, handset or speaker status.
	 *
	 * FIRST DWORD:  0x0001 = HEADSET, 0x0002 = HANDSET, 0x0003 = SPEAKERMODE
	 * SECOND DWORD: 0x0001 = OFFHOOK, 0x0002 = ONHOOK
	 * THIRD DWORD:  ALWAYS 0x0000 UNKNOWN
	 */

	struct {
		uint32_t lel_AccessoryID;					/*!< Accessory ID */
		uint32_t lel_AccessoryStatus;					/*!< Accessory Status */
		uint32_t lel_unknown;						/*!< Unknown */
	} AccessoryStatusMessage;						/*!< Accessory Status Message Structure */

	// Message 0x4A len 12 (cisco ip communicator uses it)
	// 00000000 - 03 00 00 00 01 00 00 00 00 00 00 00             ............

	struct {
		uint32_t lel_unknown1;						/*!< Unknown */
		uint32_t lel_unknown2;						/*!< Unknown */
		uint32_t lel_unknown3;						/*!< Unknown */
	} Unknown_0x004A_Message;						/*!< \todo Unknown 0x004A Message Structure */

	struct {								// INCOMPLETE
		uint32_t lel_conferenceID;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t lel_maxBitRate;					/*!< maxBitRate */
	} Unknown_0x0141_Message;						/*!< \todo Unknown 0x0141 Message Structure */

	struct {
		uint32_t lel_instance;						/*!< Instance */
		uint32_t lel_type;						/*!< always 0x15 */
		uint32_t lel_status;						/*!< status */
		char DisplayName[StationMaxNameSize];				/*!< SpeedDial Display Name \todo shoud be dynamic - readMessage - OVERRUN remaining bytes=29 messageType=0x146 */
	} FeatureStatDynamicMessage;						/*!< Speed Dial Stat Dynamic Message Structure */

	struct { // OK
                uint32_t lel_displayTimeout;
                uint32_t dummy;
        } DisplayDynamicNotifyMessage; // 0x0143

        struct { // OK
                uint32_t lel_displayTimeout;
                uint32_t lel_priority;
                uint32_t dummy;
        } DisplayDynamicPriNotifyMessage; // 0x0144;

	struct {
		uint32_t lel_messageTimeout;					/*!< Message Timeout */
		uint32_t lel_lineInstance;					/*!< Line Instance  */
		uint32_t lel_callReference;					/*!< Call Reference */
		/* here follow the message string
		 * take care rest of size should
		 * be calculated with string
		 * size + 1 (NULL TERMINATION)
		 * padded by 4
		 */
		uint32_t dummy;							/*!< Dummy, this is just for addressing, it doesn't matter */
	} DisplayDynamicPromptStatusMessage;					/*!< Display Dynamic Prompt Status Message Structure */


	struct {
		uint32_t lel_conferenceID;
		uint32_t lel_serviceNum;
	} ClearConferenceMessage;
	
	struct {
		uint32_t lel_lineNumber;					/*!< Line Number */
		uint32_t lel_lineType;						/*!< Line Type */
		uint32_t dummy;							/*!< Dummy */
	} LineStatDynamicMessage;						/*!< Line Stat Dynmic Message Structure */

	struct {
		uint32_t lel_speedDialNumber;					/*!< Speed Dial Number */
		uint32_t dummy;							/*!< Dummy */
	} Unknown_0x0149_Message;						/*!< Unknown 0x0149 Message Structure */

	struct {
		uint32_t lel_lineId;						/*!< Line ID */
		uint32_t lel_callRef;						/*!< Call Reference */
		uint32_t lel_callType;						/*!< Call Type (INBOUND=1, OUTBOUND=2, FORWARD=3) */
		uint32_t lel_originalCdpnRedirectReason;			/*!< Original CalledParty Redirect Reason */
		uint32_t lel_lastRedirectingReason;				/*!< Last Redirecting Reason */
		uint32_t lel_callInstance;					/*!< Call Instance */
		uint32_t lel_callSecurityStatus;				/*!< Call Security Status */
		uint32_t partyPIRestrictionBits;				/*!< Party PI Restriction Bits Structure
										   0 RestrictCallingPartyName
										   1 RestrictCallingPartyNumber
										   2 RestrictCalledPartyName
										   3 RestrictCalledPartyNumber
										   4 RestrictOriginalCalledPartyName
										   5 RestrictOriginalCalledPartyNumber
										   6 RestrictLastRedirectPartyName
										   7 RestrictLastRedirectPartyNumber */
		uint32_t dummy;							/*!< Dummy */
		/*
		 *      Here there are the following informations:
		 *
		 char callingParty[StationMaxDirnumSize];
		 char calledParty[StationMaxDirnumSize];
		 char originalCalledParty[StationMaxDirnumSize];
		 char lastRedirectingParty[StationMaxDirnumSize];
		 char cgpnVoiceMailbox[StationMaxDirnumSize];
		 char cdpnVoiceMailbox[StationMaxDirnumSize];
		 char originalCdpnVoiceMailbox[StationMaxDirnumSize];
		 char lastRedirectingVoiceMailbox[StationMaxDirnumSize];
		 char callingPartyName[StationMaxNameSize];
		 char calledPartyName[StationMaxNameSize];
		 char originalCalledPartyName[StationMaxNameSize];
		 char lastRedirectingPartyName[StationMaxNameSize];
		 */
	} CallInfoDynamicMessage;						/*!< Call Information Dynamic Message Structure */

	/*
	 * Unhandled SCCP Message: unknown(0x0159) 168 bytes length
	 * 00000000 - 01 00 00 00 01 00 00 00 02 00 00 00 00 00 00 00 ................
	 * 00000010 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000020 - 00 00 00 00 6D 61 78 2D 63 68 61 69 6E 65 64 3D ....max-chained=
	 * 00000030 - 32 20 64 65 76 69 63 65 4C 69 6E 65 3D 31 34 00 2 deviceLine=14.
	 * 00000040 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000050 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000060 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000070 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000080 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000090 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 000000A0 - 00 00 00 00 00 00 00 00                         ........
	 *
	 *      This was sent by a single 7970 with one CP-7914 attached.
	 *      Message is related to Addons and i suppose it notifies their
	 *      number, capacity and index.
	 */

	struct {
		char dummy[168];						/*!< Dummy */
	} Unknown_0x0159_Message;						/*!< Unknown 0x0159 Message Strucute */

	struct {
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t lel_callReference1;					/*!< Call Reference1 */
		uint32_t lel_unknown1;						/*!< Unknown (i think this switches from IPv4 to IPv6 (0x00 in IPv4)) */
		/* include IPv6 support */
		char bel_ipAddr[16];						/*!< Ip Address Array (This field is apparently in big-endian format, even though most other fields are in little-endian format.) */
		uint32_t lel_portNumber;					/*!< Port Number */
		uint32_t lel_smtStatus;						/*!< Start Media Transmission Status */
		uint32_t lel_unknown2;						/*!< Unknown 2 */
	} StartMediaTransmissionAck;						/*!< Start Media Transmission Acknowledgement Structure */

	/* this is from a 7941
	   Unhandled SCCP Message: unknown(0x0154) 44 bytes length
	   00000000 - 03 00 00 00 FC FF FF FF 03 00 00 00 00 00 00 00 ................
	   00000010 - C0 A8 09 24 00 00 00 00 00 00 00 00 00 00 00 00 ...$............
	   00000020 - 00 00 00 00 03 00 00 00 00 00 00 00             ............
	 */

	struct {
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t lel_callReference1;					/*!< Call Reference 1 */
		uint32_t lel_unknown1;						/*!< i think this switches from IPv4 to IPv6 (0x00 in IPv4) */
		/* include IPv6 support */
		char bel_ipAddr[16];						/*!< This field is apparently in big-endian format, even though most other fields are in little-endian format. */
		uint32_t lel_portNumber;					/*!< Port Number */
		uint32_t lel_smtStatus;						/*!< startmediatransmission status */
		uint32_t lel_unknown2;						/*!< Unknown */
	} StartMediaTransmissionAck_v17;					/*!< Start Media Transmission Acknowledgement used in protocoll version 17 */

	// No struct

	struct {
	} StationKeepAliveMessage;						/*!< Station Keep Alive Message */

	/* this is register message from 7940 with load 8.1.1 (protocol 0)
	 * 00000010 - 00 00 00 00 01 00 00 00 0A 0A 0A D2 07 00 00 00 ................
	 * 00000020 - 00 00 00 00 00 00 00 00 0B 00 60 85 00 00 00 00 ................
	 * 00000030 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000040 - 00 00 00 00                                     ....
	 */

	/* Register message from Cisco 7941 with load v8.5.3 (protocol 17)
	 *
	 * 00000000   53 45 50 30 30 31 46 39 45 32 36 36 33 36 39 00 SEP001F9E266369.
	 * 00000010   00 00 00 00 00 00 00 00 c0 a8 09 24 73 00 00 00 ...........$s...
	 * 00000020   05 00 00 00 00 00 00 00 11 00 72 85 01 00 00 00 ..........r.....
	 * 00000030   00 00 00 00 00 1f 9e 26 63 69 00 00 00 00 00 00 .......&ci......
	 * 00000040   03 00 00 00 02 00 00 00 00 00 00 00 00 00 00 00 ................
	 * 00000050   00 00 00 00 00 00 00 00 00 00 00 00 53 43 43 50 ............SCCP
	 * 00000060   34 31 2e 38 2d 35 2d 33 53 00 00 00 00 00 00 00 41.8-5-3S.......
	 * 00000070   00 00 00 00 00 00 00 00 00 00 00 00             ............
	 */

	/* Register message from Cisco 6961 with load v8.5.3 (protocol 15)
	 * 00000000 - 53 45 50 30 30 31 31 32 31 46 46 38 41 33 45 00 SEP001121FF8A3
	 * 00000010 - 00 00 00 00 01 00 00 00 53 DA 5E 14 F1 01 00 00 ........S.^...
	 * 00000020 - 28 00 00 00 00 00 00 00 0F 20 70 85 00 00 00 00 (........ p...
	 * 00000030 - 00 00 00 00 30 30 31 31 32 31 46 46 38 41 33 45 ....001121FF8A
	 * 00000040 - 00 00 00 00
	 */

	/* Register message from Cisco 7960 with v8.1.2 (protocol 11)
	 * 00000000 - 53 45 50 30 30 31 42 35 33 35 43 44 33 44 36 00 SEP001B535CD3D6.
	 * 00000010 - 00 00 00 00 01 00 00 00 0A 0F 0F B2 07 00 00 00 ................
	 * 00000020 - 00 00 00 00 00 00 00 00 0B 00 60 85 00 00 00 00 ................
	 * 00000030 - 00 00 00 00 00 00 00 00 00 00 00 00 52 49 4E 47 ............RING
	 * 00000040 - 00 00 00 00                                     ....
	 */     
	struct {
		StationIdentifier sId;						/*!< Station Identifier */
		uint32_t lel_stationIpAddr;	/*c0:a8:09:24=192.168.9.36*/	/*!< Station IP Address */
		uint32_t lel_deviceType;	/*73:00:00:00=115*/		/*!< Device Type as part of SKINNY_DEVICETYPE_* */
		uint32_t lel_maxStreams;	/*05:00:00:00=5*/		/*!< Max Streams */
		uint32_t lel_activeStreams;	/*00:00:00:00=0*/		/*!< number of active streams */
		uint32_t phone_features;	/*11:00:72:85	=17.*/ 		/*!< PhoneFeatures (ProtocolVersion + Features)*/
		uint32_t lel_sockettype;	/*01:00:00:00*/			/*!< MacAddress if 0x00=ASCII, 0x01=HEX */
		uint32_t lel_unknown1;		/*00:00:00:00*/			/*!< Unknown */
		uint8_t  lel_bytes1[12];	/* */				/*!< byte array */
		uint32_t lel_unknown2;
		uint32_t lel_maxButtons;
		char	 ipv6Address[16];	
		uint32_t lel_unknown3;
		char	 loadInfo[32];

		/* 7910:
		02 00 00 00 // protocolVer (1st bit)
		08 00 00 00 == 8
		00 00 00 00
		02 00 00 00 == 2
		ce f1 00 00 // == (61092 / 206 / 241) 1668 dn-size 420
		*/
	} RegisterMessage;							/*!< Register Message Structure */
	
	struct {
		StationIdentifier sId;						/*!< Station Identifier */
		uint8_t 	protocolVer;					/*!< Maximum Protocol Version */
		uint32_t 	lel_deviceType;					/*!< Device Type as part of SKINNY_DEVICETYPE_* */
		uint8_t 	lel_unknown3;
	} RegisterMessage36;							/*!< Register Message Structure */

	struct {
		// All char arrays are in multiples of 32bit
		struct {
			char deviceName[StationMaxDeviceNameSize];		/*!< Device Name */
			uint32_t lel_stationUserId;				/*!< Station User ID (Not In Use) */
			uint32_t lel_stationInstance;				/*!< Station Instance */
		} station_identifier;						/*!< Station Identifier */

		char userName[StationMaxNameSize];				/*!< User Name */
		char serverName[StationMaxNameSize];				/*!< Server Name */
		uint32_t lel_numberLines;					/*!< Number of Lines configured */
		uint32_t lel_numberSpeedDials;					/*!< Number of SpeedDials configured */
	} ConfigStatMessage;							/*!< Configuration Status Message - Server -> Client */

	struct {
		uint32_t les_rtpMediaPort;					/*!< RTP Media Port */
	} IpPortMessage;							/*!< Ip Port Message - Superseded by including the IP Port info in the OpenReceiveChannelAck Message  */

	struct {
		uint32_t lel_kpButton;						/*!< KeyPad Button */
		uint32_t lel_lineInstance;					/*!< Line Instance on device */
		uint32_t lel_callReference;					/*!< Call Reference - current channel identifier */
	} KeypadButtonMessage;							/*!< KeyPad Button Message - Client -> Server */

	struct {
		char calledParty[StationMaxDirnumSize];				/*!< Called Party */
	} EnblocCallMessage;							/*!< Enbloc Call Message - Client -> Server */

	struct {
		uint32_t lel_stimulus;						/*!< Stimulus */
		uint32_t lel_stimulusInstance;					/*!< Stimulus Instance (normally set to 1 (except speed dial and line)) */
	} StimulusMessage;							/*!< Stimulus Message - Client -> Server */

	struct {
	} OffHookMessage;							/*!< Off Hook Message Structure */

	struct {
		char callingPartyNumber[StationMaxDirnumSize];			/*!< Calling Party Number */
	} OffHookMessageWithCallingPartyNum;					/*!< Off Hook With Calling Party Number Message Structure 
										   Goes Off Hook and provides a Calling Party Number to the PBX used by multiline Devices
										 */

	struct {
	} OnHookMessage;							/*!< On Hook Message Structure */

	struct {
	} HookFlashMessage;							/*!< Hook Flash Message Structure */

	struct {
		uint32_t lel_deviceType;					/*!< Device Type as part of SKINNY_DEVICETYPE_* */
		uint32_t lel_numberOfInServiceStreams;				/*!< Number Of In-Service Streams  */
		uint32_t lel_maxStreamsPerConf;					/*!< Maximum Streams Per Configuration */
		uint32_t lel_numberOfOutOfServiceStreams;			/*!< Number of Out-Of_Service Streams */
	} MediaResourceNotification;						/*!< Media Resource Notification - Client -> Server */

	struct {
		uint32_t lel_lineNumber;					/*!< lineNumber for which Forward should be set */
	} ForwardStatReqMessage;						/*!< Forward Status Request Message - Client -> Server */

	struct {
		uint32_t lel_speedDialNumber;					/*!< instance on device */
	} SpeedDialStatReqMessage;						/*!< Speed Dial Status Request Message - Client -> Server */

	struct {
		uint32_t lel_lineNumber;					/*!< instance on device */
	} LineStatReqMessage;							/*!< Line Status Request Message - Client -> Server */

	struct {
	} ConfigStatReqMessage;							/*!< Configuration Status Request Message - Client -> Server */

	struct {
	} TimeDateReqMessage;							/*!< Time Date Request Message  - Client -> Server */

	struct {
		uint32_t lel_totalButtonCount;					/*!< number of available buttons */
	} ButtonTemplateReqMessage;						/*!< Button Template Request Message - Client -> Server  */

	struct {
	} VersionReqMessage;							/*!< Version Request Message - Client -> Server */

	struct {
		uint32_t lel_count;						/*!< Count */
		MediaCapabilityStructure caps[SKINNY_MAX_CAPABILITIES];		/*!< MediaCapabilities */
	} CapabilitiesResMessage;						/*!< Capabilities Resource Message - Client -> Server */

	struct {
		uint32_t portCount;						/*!< portCount contains the total number of RTP ports supported by the device. */
		uint32_t rtpMediaPort[StationMaxPorts];				/*!< RtpMediaPort contains the list of RTP port numbers. */
	} MediaPortListMessage;							/*!< Media Port List Message Structure
										   Used instead of the StationIpPort message for Devices which have multiple RTP stream ports. 
										   Devices like media bridges */

	struct {
	} ServerReqMessage;							/*!< Server Request Message - Client -> Server */

	struct {
		uint32_t lel_alarmSeverity;					/*!< Alarm Severity Level */
		char text[80];							/*!< Alarm Text */
		uint32_t lel_parm1;						/*!< Alarm Parameter 1 */
		uint32_t lel_parm2;						/*!< Alarm Parameter 2 */
	} AlarmMessage;								/*!< Alarm Message - Client -> Server */

	/* 0x34 FeatureStatReqMessage */
	struct {
		uint32_t lel_featureInstance;					/*!< instance on device */
		uint32_t lel_unknown;						/*!< unknown */
	} FeatureStatReqMessage;						/*!< Feature Stat Request Message - Client -> Server */

	struct {
		uint32_t lel_serviceURLIndex;					/*!< instance on device */
	} ServiceURLStatReqMessage;						/*!< Service URL Stat Request Message - Client -> Server */

	/*
	   [Jan  6 17:05:50] WARNING[29993]: sccp_actions.c:59 sccp_handle_unknown_message:
	   Unhandled SCCP Message: 48 - UpdateCapabilitiesMessage with length 1844

	   This is from a 7970 with no video caps
	   00000000 - 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000010 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000020 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000030 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000040 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000050 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000060 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000070 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000080 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000090 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000100 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000110 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000120 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000130 - 00 00 00 00 00 00 00 00 19 00 00 00 28 00 00 00 ............(...
	   00000140 - 00 00 00 00 00 00 00 00 04 00 00 00 28 00 00 00 ............(...
	   00000150 - 00 00 00 00 00 00 00 00 02 00 00 00 28 00 00 00 ............(...
	   00000160 - 00 00 00 00 00 00 00 00 0F 00 00 00 3C 00 00 00 ............<...
	   00000170 - 00 00 00 00 00 00 00 00 10 00 00 00 3C 00 00 00 ............<...
	   00000180 - 00 00 00 00 00 00 00 00 0B 00 00 00 3C 00 00 00 ............<...
	   00000190 - 00 00 00 00 00 00 00 00 0C 00 00 00 3C 00 00 00 ............<...
	   000001A0 - 00 00 00 00 00 00 00 00 01 01 00 00 01 00 00 00 ................
	   000001B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000001C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000001D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000001E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000001F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000200 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000210 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000220 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000230 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000240 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000250 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000260 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000270 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000280 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000290 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000002A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000002B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000002C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000002D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000002E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000002F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000300 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000310 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000320 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000330 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000340 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000350 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000360 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000370 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000380 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000390 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000400 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000410 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000420 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000430 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000440 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000450 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000460 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000470 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000480 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000490 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000500 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000510 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000520 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000530 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000540 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000550 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000560 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000570 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000580 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000590 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000600 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000610 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000620 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000630 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000640 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000650 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000660 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000670 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000680 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000690 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000700 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000710 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000720 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000730 - 00 00 00 00                                     ....

	   this is from a 7940 with Video Advantage on PC Port
	   00000000 - 08 00 00 00 02 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000010 - 00 00 00 00 22 4B 01 00 0C CC CC CC 00 11 21 F1 ...."K..........
	   00000020 - 22 4B 00 74 AA AA 03 00 00 0C 20 00 02 B4 B8 67 "K.t...... ....g
	   00000030 - 00 01 00 13 53 45 50 30 30 31 31 32 31 46 31 32 ....SEP001122334
	   00000040 - 32 34 42 00 02 00 11 00 00 00 01 01 01 CC 00 04 455.............
	   00000050 - 0A 0A 0A D2 00 03 00 0A 50 6F 72 74 20 31 00 04 ........Port 1..
	   00000060 - 00 08 00 00 00 90 00 05 00 10 50 30 30 33 30 38 ..........P00308
	   00000070 - 30 30 31 30 30 30 00 06 00 17 43 69 73 63 6F 20 001000....Cisco
	   00000080 - 49 50 20 50 68 6F 6E 65 20 37 39 34 00 00 00 00 IP Phone 794....
	   00000090 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000000F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000100 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000110 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000120 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000130 - 00 00 00 00 00 00 00 00 19 00 00 00 78 00 00 00 ............x...
	   00000140 - 00 00 00 00 00 00 00 00 04 00 00 00 28 00 00 00 ............(...
	   00000150 - 00 00 00 00 00 00 00 00 02 00 00 00 28 00 00 00 ............(...
	   00000160 - 00 00 00 00 00 00 00 00 0F 00 00 00 3C 00 00 00 ............<...
	   00000170 - 00 00 00 00 00 00 00 00 10 00 00 00 3C 00 00 00 ............<...
	   00000180 - 00 00 00 00 00 00 00 00 0B 00 00 00 3C 00 00 00 ............<...
	   00000190 - 00 00 00 00 00 00 00 00 0C 00 00 00 3C 00 00 00 ............<...
	   000001A0 - 00 00 00 00 00 00 00 00 01 01 00 00 04 00 00 00 ................
	   000001B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000001C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000001D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000001E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000001F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000200 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000210 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000220 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000230 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000240 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000250 - 00 00 00 00 00 00 00 00 65 00 00 00 03 00 00 00 ........e.......
	   00000260 - 02 00 00 00 32 00 00 00 02 00 00 00 98 3A 00 00 ....2........:..
	   00000270 - F4 01 00 00 01 00 00 00 00 00 00 00 33 00 00 00 ............3...
	   00000280 - 03 00 00 00 98 3A 00 00 F4 01 00 00 01 00 00 00 .....:..........
	   00000290 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000002A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000002B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000002C0 - 00 00 00 00 00 00 00 00 00 00 00 00 67 00 00 00 ............g...
	   000002D0 - 03 00 00 00 01 00 00 00 34 00 00 00 03 00 00 00 ........4.......
	   000002E0 - 98 3A 00 00 F4 01 00 00 01 00 00 00 00 00 00 00 .:..............
	   000002F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000300 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000310 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000320 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000330 - 00 00 00 00 00 00 00 00 40 00 00 00 32 00 00 00 ........@...2...
	   00000340 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000350 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000360 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000370 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000380 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000390 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000003F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000400 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000410 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000420 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000430 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000440 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000450 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000460 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000470 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000480 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000490 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000004F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000500 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000510 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000520 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000530 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000540 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000550 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000560 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000570 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000580 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000590 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000005F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000600 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000610 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000620 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000630 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000640 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000650 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000660 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000670 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000680 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000690 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006A0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006B0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006C0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006D0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006E0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   000006F0 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000700 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000710 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000720 - 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
	   00000730 - 00 00 00 00                                     ....
	 */

	struct {
		uint32_t lel_audioCapCount;					/*!< Audio Capability Count */
		uint32_t lel_videoCapCount;					/*!< Video Capability Count */
		uint32_t lel_dataCapCount;					/*!< Data Capability Count */
		uint32_t RTPPayloadFormat;					/*!< RTP Payload Format */
		uint32_t customPictureFormatCount;				/*!< Custom Picture Format Count */

		customPictureFormat_t customPictureFormat[MAX_CUSTOM_PICTURES];	/*!< Custom Picture Format */

		uint32_t activeStreamsOnRegistration;				/*!< Active Streams on Registration */
		uint32_t maxBW;							/*!< Max BW ?? */

		uint32_t serviceResourceCount;					/*!< Service Resource Count */
		serviceResource_t serviceResource[MAX_SERVICE_TYPE];		/*!< Service Resource */

		audioCap_t audioCaps[SKINNY_MAX_CAPABILITIES];			/*!< Audio Capabilities */
		videoCap_t videoCaps[SKINNY_MAX_VIDEO_CAPABILITIES];		/*!< Video Capabilities */
		dataCap_t dataCaps[SKINNY_MAX_DATA_CAPABILITIES];		/*!< Data Capabilities */

		uint32_t unknown;						/*!< Unknown */
	} UpdateCapabilitiesMessage;						/*!< Update Capabilities Message Structure */

	struct {
		uint32_t receptionStatus;					/*!< Reception Status */
		uint32_t passThruPartyID;					/*!< Pass Through Party ID */
	} MulticastMediaReceptionAck;						/*!< Multicast Media Reception Acknowledgement Message - Client -> Server */

	struct {
		uint32_t lel_orcStatus;						/*!< receiveChanStatus */
		uint32_t bel_ipAddr;						/*!< This field is apparently in big-endian format,
										   even though most other fields are in
										   little-endian format. */
		uint32_t lel_portNumber;					/*!< Port Number */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t lel_callReference;					/*!< Call Reference */
	} OpenReceiveChannelAck;						/*!< Open Receive Channel Acknowledgement */

	/*      this is v17 open receive channel ack
	 *      0000   28 00 00 00 11 00 00 00 22 00 00 00 ST AT US xx  (.......".......
	 *      0010   00 00 00 00 IP IP IP IP 00 00 00 00 00 00 00 00  ......."........
	 *      0020   00 00 00 00 3a 4c 00 00 53 00 00 01 54 ae 9d 01  ....:L..S...T...
	 */

	struct {
		uint32_t lel_orcStatus;						/*!< Receive Channel Status */
		uint32_t lel_unknown1;						/*!< I think this switches from IPv4 to IPv6 (0x00 in IPv4) */
		/* include IPv6 support */
		char bel_ipAddr[16];						/*!< This field is apparently in big-endian format,
										   even though most other fields are in
										   little-endian format. */
		uint32_t lel_portNumber;					/*!< Port Number */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t lel_callReference;					/*!< Call Reference */
	} OpenReceiveChannelAck_v17;						/*!< Open Receive Channel Acknowledgement v17 */

	struct {
		uint32_t lel_orcStatus;						/*!< receiveChanStatus */
		uint32_t bel_ipAddr;						/*!< This field is apparently in big-endian format,
										   even though most other fields are in
										   little-endian format. */
		uint32_t lel_portNumber;
		uint32_t lel_passThruPartyId;
		uint32_t lel_callReference;

	} OpenMultiMediaReceiveChannelAckMessage;

	struct {
		uint32_t lel_orcStatus;						/*!< status */
		uint32_t lel_unknown1;						/*!< I think this switches from IPv4 to IPv6 (0x00 in IPv4) */
		/* include IPv6 support */
		char bel_ipAddr[16];						/*!< This field is apparently in big-endian format,
										   even though most other fields are in
										   little-endian format. */
		uint32_t lel_portNumber;
		uint32_t lel_passThruPartyId;
		uint32_t lel_callReference;

	} OpenMultiMediaReceiveChannelAckMessage_v17;

	struct {
		char DirectoryNumber[StationMaxDirnumSize];			/*!< Directory Number */
		uint32_t lel_CallIdentifier;					/*!< Call Identifier */
		uint32_t lel_StatsProcessingType;				/*!< Stats Processing Type */
		uint32_t lel_SentPackets;					/*!< Sent Packets */
		uint32_t lel_SentOctets;					/*!< Sent Octets */
		uint32_t lel_RecvdPackets;					/*!< Received Packets */
		uint32_t lel_RecvdOctets;					/*!< Received Octets */
		uint32_t lel_LostPkts;						/*!< Lost Packets */
		uint32_t lel_Jitter;						/*!< Jitter */
		uint32_t lel_latency;						/*!< Latency */
	} ConnectionStatisticsRes;						/*!< Connection Statistics Response Message - Server -> Client */
	
	struct {
		char DirectoryNumber[28];					/*!< Directory Number */
		uint32_t lel_CallIdentifier;					/*!< Call Identifier */
// 		uint32_t lel_StatsProcessingType;				/*!< Stats Processing Type */
		uint32_t lel_SentPackets;					/*!< Sent Packets */
		uint32_t lel_SentOctets;					/*!< Sent Octets */
		uint32_t lel_RecvdPackets;					/*!< Received Packets */
		uint32_t lel_RecvdOctets;					/*!< Received Octets */
		uint32_t lel_LostPkts;						/*!< Lost Packets */
		uint32_t lel_Jitter;						/*!< Jitter */
		uint32_t lel_latency;						/*!< Latency */
		uint8_t  bytes[2];						/*!< Latency */
	} ConnectionStatisticsRes_V19;						/*!< Connection Statistics Response Message - Client -> Server (used when protocol version >= 19) */

	struct {
		char calledParty[StationMaxDirnumSize];				/*!< Called Party Array */
	} OffHookWithCgpnMessage;						/*!< Off Hook With Calling Party Name Message Structure */

	struct {
	} SoftKeySetReqMessage;							/*!< Soft Key Set Request Message Structure */

	struct {
		uint32_t lel_softKeyEvent;					/*!< Soft Key Event */
		uint32_t lel_lineInstance;					/*!< Line Instance on device (indicated callplane) */
		uint32_t lel_callReference;					/*!< Call Reference ID */
	} SoftKeyEventMessage;

	struct {
	} UnregisterMessage;							/*!< Unregister Message Structure */

	struct {
	} SoftKeyTemplateReqMessage;						/*!< Soft Key Template Request Message Structure */

	struct {
		StationIdentifier sId;						/*!< Station Identifier */
		uint32_t lel_stationIpAddr;					/*!< Station IP Address */
		uint32_t lel_deviceType;					/*!< Device Type as part of SKINNY_DEVICETYPE_* */
		char	 ipv6Address[16];
		uint32_t unknown;
	} RegisterTokenReq;							/*!< Register Token Request */

	struct {
		uint32_t lel_hsMode;						/*!< Head Set Mode */
	} HeadsetStatusMessage;							/*!< Headset Status Message Structure */

	struct {
		uint32_t lel_keepAliveInterval;					/*!< Keep Alive Interval to the Primary Server */
		char dateTemplate[StationDateTemplateSize];			/*!< Date Template */
		uint8_t filler1;						/*!< Filler 1 */
		uint8_t filler2;						/*!< Filler 2 */
		uint32_t lel_secondaryKeepAliveInterval;			/*!< Keep Alive Interval to the Secundairy Server */
		uint8_t protocolVer;						/*!< Protocol Version */
		uint8_t unknown1;						/*!< Unknown */
		uint8_t unknown2;						/*!< Unknown */
		uint8_t unknown3;						/*!< Unknown */
	} RegisterAckMessage;							/*!< Register Acknowledgement Message Structure */

	struct {
		uint32_t lel_tone;						/*!< Tone */
		uint32_t lel_toneTimeout;					/*!< Tone Timeout */
		uint32_t lel_lineInstance;					/*!< Line Instance */
		uint32_t lel_callReference;					/*!< Call Reference */
	} StartToneMessage;							/*!< Start Tone Message Structure */

	struct {
		uint32_t lel_lineInstance;					/*!< Line Instance */
		uint32_t lel_callReference;					/*!< Call Reference */
		/* protocol v12 mod */
		uint32_t lel_unknown1;						/*!< Unknown */
	} StopToneMessage;

	struct {
		uint32_t lel_ringMode;						/*!< Ring Mode */
		uint32_t lel_unknown1;						/*!< Unknown (always 1) */
		uint32_t lel_lineInstance;					/*!< Line Instance */
		uint32_t lel_callReference;					/*!< Call Reference */
	} SetRingerMessage;							/*!< Set Ringer Message Structure */

	struct {
		uint32_t lel_stimulus;						/*!< Stimulus */
		uint32_t lel_stimulusInstance;					/*!< Stimulus Instance */
		uint32_t lel_lampMode;						/*!< Lamp Mode (MWI) */
	} SetLampMessage;							/*!< Set Lamp Message Structure */

	struct {
	} SetHkFDetectMessage;							/*!< Set Hkf Detect Message Structure */

	struct {
		uint32_t lel_speakerMode;					/*!< Spreaker Mode */
	} SetSpeakerModeMessage;						/*!< Set Speaker Mode Message Structure */

	struct {
		uint32_t lel_micMode;						/*!< Microphone Mode */
	} SetMicroModeMessage;							/*!< Set Microphone Mode Message Structure */

	struct {
		uint32_t lel_conferenceId;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t bel_remoteIpAddr;					/*!< This field is apparently in big-endian
										   format, even though most other fields are
										   little-endian. */
		uint32_t lel_remotePortNumber;					/*!< Remote Port Number */
		uint32_t lel_millisecondPacketSize;				/*!< Packet Size per MilliSecond */
		uint32_t lel_payloadType;					/*!< Media_PayloadType */
		uint32_t lel_precedenceValue;					/*!< Precedence Value */
		uint32_t lel_ssValue;						/*!< Simple String Value */
		uint32_t lel_maxFramesPerPacket;				/*!< Maximum Frames per Packet */
		uint32_t lel_g723BitRate;					/*!< only used with G.723 payload */
		uint32_t lel_conferenceId1;					/*!< Conference ID 1 */
		uint32_t unknown1;						/*!< Unknown */
		uint32_t unknown2;						/*!< Unknown */
		uint32_t unknown3;						/*!< Unknown */
		uint32_t unknown4;						/*!< Unknown */
		uint32_t unknown5;						/*!< Unknown */
		uint32_t unknown6;						/*!< Unknown */
		uint32_t unknown7;						/*!< Unknown */
		uint32_t unknown8;						/*!< Unknown */
		uint32_t unknown9;						/*!< Unknown */
		uint32_t unknown10;						/*!< Unknown */
		/* protocol v11 mods */
		uint32_t unknown11;						/*!< Unknown */
		uint32_t unknown12;						/*!< Unknown */
		uint32_t unknown13;						/*!< Unknown */
		uint32_t unknown14;						/*!< Unknown */
		uint32_t lel_rtpDTMFPayload;					/*!< RTP DTMP PayLoad (this is often set to 0x65 (101)) */
		uint32_t lel_rtptimeout;					/*!< RTP TimeOut */
		/* protocol v11 fields */
		uint32_t unknown15;
		uint32_t unknown16;
	} StartMediaTransmission;

	/* StartMediaTransmission v17
	 * 0000   88 00 00 00 11 00 00 00 8a 00 00 00 54 ae 9d 01  ............T...
	 * 0010   53 00 00 01 00 00 00 00 c0 a8 09 2c 00 00 00 00  S..........,....
	 * 0020   00 00 00 00 00 00 00 00 c6 4c 00 00 14 00 00 00  .........L......
	 * 0030   04 00 00 00 b8 00 00 00 00 00 00 00 00 00 00 00  ................
	 * 0040   00 00 00 00 54 ae 9d 01 00 00 00 00 00 00 00 00  ....T...........
	 * 0050   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
	 * 0060   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
	 * 0070   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
	 * 0080   00 00 00 00 0a 00 00 00 00 00 00 00 00 00 00 00  ................
	 */

	struct {
		uint32_t lel_conferenceId;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t lel_unknown1;						/*!< Unknown */
		char bel_remoteIpAddr[16];					/*!< This field is apparently in big-endian
										   format, even though most other fields are
										   little-endian. */
		uint32_t lel_remotePortNumber;					/*!< Remote Port Number */
		uint32_t lel_millisecondPacketSize;				/*!< Packet Size per Millisecond */
		uint32_t lel_payloadType;					/*!< Media_PayloadType */
		uint32_t lel_precedenceValue;					/*!< Precedence Value */
		uint32_t lel_ssValue;						/*!< Simple String Value */
		uint32_t lel_maxFramesPerPacket;				/*!< Maximum Frames per Packet */
		uint32_t lel_g723BitRate;					/*!< G.723 BitRate (only used with G.723 payload) */
		uint32_t lel_conferenceId1;					/*!< Conference ID 1 */
		uint32_t lel_unknown2;						/*!< Unknown */
		uint32_t lel_unknown3;						/*!< Unknown */
		uint32_t lel_unknown4;						/*!< Unknown */
		uint32_t lel_unknown5;						/*!< Unknown */
		uint32_t lel_unknown6;						/*!< Unknown */
		uint32_t lel_unknown7;						/*!< Unknown */
		uint32_t lel_unknown8;						/*!< Unknown */
		uint32_t lel_unknown9;						/*!< Unknown */
		uint32_t lel_unknown10;						/*!< Unknown */
		uint32_t lel_unknown11;						/*!< Unknown */
		uint32_t lel_unknown12;						/*!< Unknown */
		uint32_t lel_unknown13;						/*!< Unknown */
		uint32_t lel_unknown14;						/*!< Unknown */
		uint32_t lel_unknown15;						/*!< Unknown */
		uint32_t lel_rtpDTMFPayload;					/*!< RTP DTMP PayLoad (this is often set to 0x65 (101)) */
		uint32_t lel_rtptimeout;					/*!< RTP Timeout (this is set to 0x0A) */
		uint32_t lel_unknown18;						/*!< Unknown */
		uint32_t lel_unknown19;						/*!< Unknown */
	} StartMediaTransmission_v17;						/*!< Start Media Transmission v17 Structure */

	struct {
		uint32_t lel_conferenceId;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t lel_conferenceId1;					/*!< Conference ID 1 */
		uint32_t lel_unknown1;						/*!< Unknown (proto v17) */
	} StopMediaTransmission;						/*!< Stop Media Transmission Structure */

	struct {
		uint32_t lel_conferenceId;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t lel_conferenceId1;					/*!< Conference ID 1 */
		uint32_t lel_unknown1;						/*!< Unknown (proto v17) */
	} StopMultiMediaTransmission;

	struct {
	} StartMediaReception;

	struct {
		uint32_t lel_conferenceId;					/*!< Conference ID 1 */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
	} StopMediaReception;							/*!< Stop Media Reception Structure */

	struct {
		char callingPartyName[StationMaxNameSize];			/*!< Calling Party Name */
		char callingParty[StationMaxDirnumSize];			/*!< Calling Party ID */
		char calledPartyName[StationMaxNameSize];			/*!< Called Party Name */
		char calledParty[StationMaxDirnumSize];				/*!< Called Party ID */
		uint32_t lel_lineId;						/*!< Line ID */
		uint32_t lel_callRef;						/*!< Call Reference */
		uint32_t lel_callType;						/*!< Call Type (INBOUND=1, OUTBOUND=2, FORWARD=3) */
		char originalCalledPartyName[StationMaxNameSize];		/*!< Original Calling Party Name */
		char originalCalledParty[StationMaxDirnumSize];			/*!< Original Calling Party ID */
		char lastRedirectingPartyName[StationMaxNameSize];		/*!< Original Called Party Name */
		char lastRedirectingParty[StationMaxDirnumSize];		/*!< Original Called Party ID */
		uint32_t originalCdpnRedirectReason;				/*!< Original Called Party Redirect Reason */
		uint32_t lastRedirectingReason;					/*!< Last Redirecting Reason */
		char cgpnVoiceMailbox[StationMaxDirnumSize];			/*!< Calling Party Voicemail Box */
		char cdpnVoiceMailbox[StationMaxDirnumSize];			/*!< Called Party Voicemail Box */
		char originalCdpnVoiceMailbox[StationMaxDirnumSize];		/*!< Original Called Party VoiceMail Box */
		char lastRedirectingVoiceMailbox[StationMaxDirnumSize];		/*!< Last Redirecting VoiceMail Box */
		uint32_t lel_callInstance;					/*!< Call Instance */
		uint32_t lel_callSecurityStatus;				/*!< Call Security Status */
		uint32_t partyPIRestrictionBits;				/*!< Party PI Restriction Bits (
										   0 RestrictCallingPartyName
										   1 RestrictCallingPartyNumber
										   2 RestrictCalledPartyName
										   3 RestrictCalledPartyNumber
										   4 RestrictOriginalCalledPartyName
										   5 RestrictOriginalCalledPartyNumber
										   6 RestrictLastRedirectPartyName
										   7 RestrictLastRedirectPartyNumber) */
	} CallInfoMessage;							/*!< Call Info Message Structure */

	struct {
		char calledParty[StationMaxDirnumSize];				/*!< Called Party */
		uint32_t lel_lineId;						/*!< Line ID */
		uint32_t lel_callRef;						/*!< Call Reference */
	} DialedNumberMessage;							/*!< Dialed Number Message Structure */
	
	struct {
		char calledParty[25];						/*!< Called Party */
		uint32_t lel_lineId;						/*!< Line ID */
		uint32_t lel_callRef;						/*!< Call Reference */
		uint8_t	 padding1[3];
	} DialedNumberMessageV19;						/*!< Dialed Number Message Structure -TEST*/

	struct {
		uint32_t lel_status;						/*!< Status (0=inactive, 1=active) */
		uint32_t lel_lineNumber;					/*!< Line Number */
		uint32_t lel_cfwdallstatus;					/*!< Call Forward All Status */
		char cfwdallnumber[StationMaxDirnumSize];			/*!< Call Forward All Number */
		uint32_t lel_cfwdbusystatus;					/*!< Call Forward on Busy Status */
		char cfwdbusynumber[StationMaxDirnumSize];			/*!< Call Forward on Busy Number */
		uint32_t lel_cfwdnoanswerstatus;				/*!< Call Forward on No-Answer Status */
		char cfwdnoanswernumber[StationMaxDirnumSize];			/*!< Call Forward on No-Answer Number */
	} ForwardStatMessage;							/*!< Forward Status Message Structure */
	
	struct {
		uint32_t lel_status;						/*!< Status (0=inactive, 1=active) */
		uint32_t lel_lineNumber;					/*!< Line Number */
		uint32_t lel_cfwdallstatus;					/*!< Call Forward All Status */
		char cfwdallnumber[StationMaxDirnumSize];			/*!< Call Forward All Number */
		uint32_t lel_cfwdbusystatus;					/*!< Call Forward on Busy Status */
		char cfwdbusynumber[StationMaxDirnumSize];			/*!< Call Forward on Busy Number */
		uint32_t lel_cfwdnoanswerstatus;				/*!< Call Forward on No-Answer Status */
		char cfwdnoanswernumber[StationMaxDirnumSize];			/*!< Call Forward on No-Answer Number */
		uint32_t lel_unknown;						/*!< 00 00 00 ff */
	} ForwardStatMessageV19;						/*!< Forward Status Message Structure */

	struct {
		uint32_t lel_speedDialNumber;					/*!< SpeedDial Number */
		char speedDialDirNumber[StationMaxDirnumSize];			/*!< SpeedDial Dir Number */
		char speedDialDisplayName[StationMaxNameSize];			/*!< SpeedDial Display Name */
	} SpeedDialStatMessage;							/*!< SpeedDial Status Message Structure */

	struct {
		uint32_t lel_lineNumber;					/*!< Line Number */
		char lineDirNumber[StationMaxDirnumSize];			/*!< Line Dir Number */
		char lineFullyQualifiedDisplayName[StationMaxNameSize];		/*!< Line Fully Qualified Display Name */
		char lineDisplayName[StationMaxButtonTemplateNameSize];		/*!< Line Display Name */
	} LineStatMessage;							/*!< Line Status Messages Structure */

	struct {
		uint32_t lel_year;						/*!< Year */
		uint32_t lel_month;						/*!< Month */
		uint32_t lel_dayOfWeek;						/*!< Day of the Week (\todo Starting su=0|1) */
		uint32_t lel_day;						/*!< Day */
		uint32_t lel_hour;						/*!< Hour */
		uint32_t lel_minute;						/*!< Minute */
		uint32_t lel_seconds;						/*!< Seconds */
		uint32_t lel_milliseconds;					/*!< MilliSeconds */
		uint32_t lel_systemTime;					/*!< System Time */
	} DefineTimeDate;							/*!< Definition of Date/Time Structure */

	struct {
	} StartSessionTransmission;						/*!< Start Session Transmission Structure */
	struct {
	} StopSessionTransmission;						/*!< Stop Session Transmission Structure */

	struct {
		uint32_t lel_buttonOffset;					/*!< Button OffSet */
		uint32_t lel_buttonCount;					/*!< Button Count */
		uint32_t lel_totalButtonCount;					/*!< Total Number of Buttons */
		StationButtonDefinition definition[StationMaxButtonTemplateSize];	/*!< Station Button Definition */
	} ButtonTemplateMessage;						/*!< Button Template Message Structure */

	struct {
		uint32_t lel_buttonOffset;					/*!< Button OffSet */
		uint32_t lel_buttonCount;					/*!< Button Count */
		uint32_t lel_totalButtonCount;					/*!< Total Number of Buttons */
		StationButtonDefinition definition[1];				/*!< Station Button Definition */
	} ButtonTemplateMessageSingle;						/*!< Button Template Message Structure */

	struct {
		char requiredVersion[StationMaxVersionSize];			/*!< Required Version */
	} VersionMessage;							/*!< Version Message Structure */

	struct {
		char displayMessage[StationMaxDisplayTextSize];			/*!< Display Message */
		/*              uint32_t        lel_displayTimeout; *//*!< Display Timeout */
	} DisplayTextMessage;							/*!< Display Text Message */

	struct {
		uint32_t unknown;
	} ClearDisplay;

	struct {
	} CapabilitiesReqMessage;						/*!< Capabilities Reqest Message Structure */
	struct {
	} EnunciatorCommandMessage;						/*!< Enunciator Command Message Structure */

	struct {
		char text[StationMaxDisplayTextSize];				/*!< Rejection Text */
	} RegisterRejectMessage;						/*!< Register Reject Message Structure */

	struct {
		ServerIdentifier server[StationMaxServers];			/*!< Server Identifier */
		uint32_t serverListenPort[StationMaxServers];			/*!< Server is Listening on Port */
		uint32_t serverIpAddr[StationMaxServers];			/*!< Server IP Port */
	} ServerResMessage;							/*!< Server Result Message Structure */

	struct {
		uint32_t lel_resetType;						/*!< Reset Type (1=Reset, 2=Restart) */
	} Reset;								/*!< Reset Message Structure */

	struct {
	} KeepAliveAckMessage;							/*!< Keep Aliver Acknowledgement Message Structure */

	struct {
		uint32_t lel_conferenceID;					/*!< Conference ID */
		uint32_t lel_passThruPartyID;					/*!< Pass Through Party ID */
		uint32_t lel_multicastIpAddress;				/*!< Multicast IP Address */
		uint32_t lel_multicastPort;					/*!< Multicast Port */
		uint32_t lel_millisecondPacketSize;				/*!< Millisecond Packet Size */
		skinny_codec_t lel_payloadCapability;				/*!< PayLoad Capability */
		uint32_t lel_echoCancelType;					/*!< Echo Cancelation Type */
		uint32_t lel_g723BitRate;					/*!< G.723 Bit Rate (only applies to G.723 */
	} StartMulticastMediaReception;						/*!< Start Multicast Media Reception Message Structure */

	struct {
		uint32_t lel_conferenceID;					/*!< Conference ID */
		uint32_t lel_passThruPartyID;					/*!< Pass Through Party ID */
		uint32_t lel_multicastIpAddress;				/*!< Multicast IP Address */
		uint32_t lel_multicastPort;					/*!< Multicast Port */
		uint32_t lel_millisecondPacketSize;				/*!< Millisecond Packet Size */
		skinny_codec_t lel_payloadCapability;				/*!< PayLoad Capability */
		uint32_t lel_precedenceValue;					/*!< Precedence Value */
		uint32_t lel_silenceSuppression;				/*!< Silence Suppression */
		uint32_t lel_maxFramesPerPacket;				/*!< Max Frames Per Packet */
		uint32_t lel_g723BitRate;					/*!< G.723 Bit Rate (only applies to G.723 */
	} StartMulticastMediaTransmission;					/*!< Start Multicast Media Transmission Message Structure */

	/*
	 * based on miscCommandType we have other struct
	 * following the Miscellaneous Command Message
	 */

	struct {
		uint32_t lel_conferenceId;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t lel_miscCommandType;					/*!< Miscellaneous Command Type */
		uint32_t unknown[10];
	} MiscellaneousCommandMessage;						/*!< Miscellaneous Command Message Structure */

	struct {
		uint32_t lel_conferenceID;					/*!< Conference ID */
		uint32_t lel_passThruPartyID;					/*!< Pass Through Party ID */
	} StopMulticastMediaReception;						/*!< Stop Multicast Media Reception Message Structure */

	struct {
		uint32_t lel_conferenceID;					/*!< Conference ID */
		uint32_t lel_passThruPartyID;					/*!< Pass Through Party ID */
	} StopMulticastMediaTransmission;					/*!< Stop Multicast Media Transmission Message Structure */

	/* this is v11 message from ccm7
	 * 0000   60 00 00 00 00 00 00 00 05 01 00 00 5b ae 9d 01  `...........[...
	 * 0010   5a 00 00 01 14 00 00 00 04 00 00 00 00 00 00 00  Z...............
	 * 0020   00 00 00 00 5b ae 9d 01 00 00 00 00 00 00 00 00  ....[...........
	 * 0030   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
	 * 0040   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
	 * 0050   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
	 * 0060   00 00 00 00 0a 00 00 00                          ........
	 */

	struct {
		uint32_t lel_conferenceId;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t lel_millisecondPacketSize;				/*!< Millisecond Packet Size */
		uint32_t lel_payloadType;					/*!< Media_Payload Type */
		uint32_t lel_vadValue;						/*!< VAD Value */
		uint32_t lel_g723BitRate;					/*!< G.723 Payload (Only applies to G.723) */
		/* protocol version 5 fields */
		uint32_t lel_conferenceId1;					/*!< Conference ID */
		uint32_t unknown1;						/*!< Unknown */
		uint32_t unknown2;						/*!< Unknown */
		uint32_t unknown3;						/*!< Unknown */
		uint32_t unknown4;						/*!< Unknown */
		uint32_t unknown5;						/*!< Unknown */
		uint32_t unknown6;						/*!< Unknown */
		uint32_t unknown7;						/*!< Unknown */
		uint32_t unknown8;						/*!< Unknown */
		uint32_t unknown9;						/*!< Unknown */
		uint32_t unknown10;						/*!< Unknown */
		/* protocol version 11 fields */
		uint32_t unknown11;						/*!< Unknown */
		uint32_t unknown12;						/*!< Unknown */
		uint32_t unknown13;						/*!< Unknown */
		uint32_t unknown14;						/*!< Unknown */
		uint32_t lel_rtpDTMFPayload;					/*!< RTP DTMF PayLoad (this is often set to 0x65 (101)) */
		uint32_t lel_rtptimeout;					/*!< RTP Timeout (this is always 0x0A) */
		/* protocol version 15 fields */
		uint32_t unknown15;
		uint32_t unknown16;
		char bel_remoteIpAddr[16];
		uint32_t lel_unknown17;						/*!< this is always 0xFA0 */
	} OpenReceiveChannel;							/*!< Open Receive Channel Message Structure */

	/*      OpenReceiveChannel v17
	 * 0000   84 00 00 00 11 00 00 00 05 01 00 00 ec bc 68 01  ..............h.
	 * 0010   07 00 00 01 14 00 00 00 04 00 00 00 00 00 00 00  ................
	 * 0020   00 00 00 00 ec bc 68 01 00 00 00 00 00 00 00 00  ......h.........
	 * 0030   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
	 * 0040   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
	 * 0050   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
	 * 0060   00 00 00 00 0a 00 00 00 00 00 00 00 00 00 00 00  ................
	 * 0070   00 00 00 00 c0 a8 09 2c 00 00 00 00 00 00 00 00  .......,........
	 * 0080   00 00 00 00 a0 0f 00 00 00 00 00 00              ............
	 */

	struct {
		uint32_t lel_conferenceId;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t lel_millisecondPacketSize;				/*!< Millisecond Packet Size */
		uint32_t lel_payloadType;					/*!< Media_Payload Type */
		uint32_t lel_vadValue;						/*!< VAD Value */
		uint32_t lel_g723BitRate;					/*!< G.723 Payload (Only applies to G.723) */
		/* protocol version 5 fields */
		uint32_t lel_conferenceId1;					/*!< Conference ID */
		uint32_t unknown1;						/*!< Unknown */
		uint32_t unknown2;						/*!< Unknown */
		uint32_t unknown3;						/*!< Unknown */
		uint32_t unknown4;						/*!< Unknown */
		uint32_t unknown5;						/*!< Unknown */
		uint32_t unknown6;						/*!< Unknown */
		uint32_t unknown7;						/*!< Unknown */
		uint32_t unknown8;						/*!< Unknown */
		uint32_t unknown9;						/*!< Unknown */
		uint32_t unknown10;						/*!< Unknown */
		uint32_t unknown11;						/*!< Unknown */
		uint32_t unknown12;						/*!< Unknown */
		uint32_t unknown13;						/*!< Unknown */
		uint32_t unknown14;						/*!< Unknown */
		uint32_t lel_rtpDTMFPayload;					/*!< RTP DTMF PayLoad (this is often set to 0x65 (101)) */
		uint32_t lel_rtptimeout;					/*!< RTP Timeout (this is always 0x0A) */
		uint32_t unknown17;						/*!< Unknown */
		uint32_t unknown18;						/*!< Unknown */
		uint32_t unknown19;						/*!< Unknown */
		char bel_remoteIpAddr[16];					/*!< Remote IP Address */
		uint32_t lel_unknown20;						/*!< Unknown (this is always 0xFA0) */
		uint32_t unknown21;						/*!< Unknown */
	} OpenReceiveChannel_v17;						/*!< Open Receive Channel Message Structure v17 */

	struct {
		uint32_t lel_conferenceId;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		/* version 5 fields */
		uint32_t lel_conferenceId1;					/*!< Conference ID 1 */
	} CloseReceiveChannel;							/*!< Close Receive Channel */

	struct {
		uint32_t lel_conferenceId;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		/* version 5 fields */
		uint32_t lel_conferenceId1;					/*!< Conference ID 1 */
	} CloseMultiMediaReceiveChannel;

	struct {								// Request Statistics from Phone
		char DirectoryNumber[StationMaxDirnumSize];			/*!< Directory Number */
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t lel_StatsProcessing;					/*!< Statistics Processing */
	} ConnectionStatisticsReq;						/*!< Connection Statistics Request Message Structure */
	
	struct {								// Request Statistics from Phone
		char DirectoryNumber[StationMaxDirnumSize];			/*!< Directory Number */
		uint8_t  byte;
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t lel_StatsProcessing;					/*!< Statistics Processing */
	} ConnectionStatisticsReq_V19;						/*!< Connection Statistics Request Message Structure */

	struct {
		uint32_t lel_softKeyOffset;					/*!< Soft Key Off Set */
		uint32_t lel_softKeyCount;					/*!< Soft Key Count */
		uint32_t lel_totalSoftKeyCount;					/*!< Total Number of Soft Keys */
		StationSoftKeyDefinition definition[1];				/*!< Station Soft Key Definition (dummy) */
	} SoftKeyTemplateResMessage;						/*!< Soft Key Template Result Message Structure */

	struct {
		uint32_t lel_softKeySetOffset;					/*!< Soft Key Off Set */
		uint32_t lel_softKeySetCount;					/*!< Soft Key Count */
		uint32_t lel_totalSoftKeySetCount;				/*!< Total Number of Soft Keys */
		StationSoftKeySetDefinition definition[StationMaxSoftKeySetDefinition];	/*!< Station Soft Key Definition */
	} SoftKeySetResMessage;							/*!< Soft Key Set Result Message Structure */

	struct {
		uint32_t lel_lineInstance;					/*!< Line Instance */
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t lel_softKeySetIndex;					/*!< Soft Key Set Index */
		uint32_t les_validKeyMask;					/*!< Valid Key Mask */
	} SelectSoftKeysMessage;						/*!< Select Soft Keys Message Structure */

	struct {
		uint32_t lel_callState;						/*!< Call State */
		uint32_t lel_lineInstance;					/*!< Line Instance */
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t lel_visibility;					/*!< Visibility */
		uint32_t lel_priority;						/*!< Priority */
		uint32_t lel_unknown3;						/*!< Unknown */
	} CallStateMessage;							/*!< Call State Message Structure */

	struct {
		uint32_t lel_messageTimeout;					/*!< Message Timeout */
		char promptMessage[32];						/*!< Prompt Message (Max Lenght 32) */
		uint32_t lel_lineInstance;					/*!< Line Instance */
		uint32_t lel_callReference;					/*!< Call Reference */
	} DisplayPromptStatusMessage;						/*!< Display Prompt Status Message Structure */

	struct {
		uint32_t lel_lineInstance;					/*!< Line Instance */
		uint32_t lel_callReference;					/*!< Call Reference */
	} ClearPromptStatusMessage;						/*!< Clear Prompt Status Message Structure */

	struct {
		uint32_t lel_displayTimeout;					/*!< Display Timeout */
		char displayMessage[StationMaxDisplayNotifySize];		/*!< Display Message */
	} DisplayNotifyMessage;							/*!< Display Notify Message Structure */

	struct {
	} ClearNotifyMessage;							/*!< Clear Notify Message Structure */

	/* 0x11F FeatureStatMessage */
	struct {
		uint32_t lel_featureInstance;					/*!< Feature Instance */
		uint32_t lel_featureID;						/*!< Feature ID */
		char featureTextLabel[StationMaxNameSize];			/*!< Feature Text Label */
		uint32_t lel_featureStatus;					/*!< Feature Status */
	} FeatureStatMessage;							/*!< Feature Status Message Structure */

	struct {
		uint32_t lel_serviceURLIndex;					/*!< Service URL Index */
		char URL[StationMaxServiceURLSize];				/*!< Actual URL */
		char label[StationMaxNameSize];					/*!< Label */
	} ServiceURLStatMessage;						/*!< Service URL Stat Message Structure */

	struct {								// Used Above Protocol 7 */
		uint32_t lel_serviceURLIndex;					/*!< Service URL Index */
		uint32_t dummy;							/*!< Dummy */
	} ServiceURLStatDynamicMessage;						/*!< Service URL Stat Message Structure */

	struct {
		uint32_t lel_status;						/*!< Status */
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t lel_lineInstance;					/*!< Line Instance */
	} CallSelectStatMessage;						/*!< Call Select Status Message Structure */

// 	struct {
// 		uint32_t lel_conferenceID;					/*!< Conference ID */
// 		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
// 		skinny_codec_t lel_payloadCapability;				/*!< payload capability */
// 		uint32_t lel_lineInstance;					/*!< Line Instance */
// 		uint32_t lel_callReference;					/*!< Call Reference */
// 		uint32_t lel_payload_rfc_number;				/*!<  */
// 		uint32_t lel_payloadType;					/*!< payload type */
// 		uint32_t lel_isConferenceCreator;				/*!< we can set it to 0 */
// 
// 		audioParameter_t audioParameter;				/*!< Audio Parameter */
// 		videoParameter_t videoParameter;				/*!< Video Parameter */
// 		dataParameter_t dataParameter;					/*!< Data Parameter */
// 
// 		uint32_t unknown[12];						/*!< Unknown */
// 	} OpenMultiMediaChannelMessage;						/*!< Open Multi Media Channel Message Structure */
// 
// 	struct {
// 		uint32_t lel_conferenceID;					/*!< Conference ID */
// 		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
// 		skinny_codec_t lel_payloadCapability;				/*!< payload capability */
// 		uint32_t lel_lineInstance;					/*!< Line Instance */
// 		uint32_t lel_callReference;					/*!< Call Reference */
// 		uint32_t lel_payload_rfc_number;				/*!<  */
// 		uint32_t lel_payloadType;					/*!< payload type */
// 		uint32_t lel_isConferenceCreator;				/*!< we can set it to 0 */
// 
// 		audioParameter_t audioParameter;				/*!< Audio Parameter */
// 		videoParameter_t videoParameter;				/*!< Video Parameter */
// 		dataParameter_t dataParameter;					/*!< Data Parameter */
// 
// 		uint32_t unknown[19];						/*!< Unknown */
// 	} OpenMultiMediaChannelMessage_v17;					/*!< Open Multi Media Channel Message Structure */


	struct {
		uint32_t lel_conferenceID;                                      /*!< Conference ID */
		uint32_t lel_passThruPartyId;                                   /*!< Pass Through Party ID */
		skinny_codec_t lel_payloadCapability;                     /*!< payload capability */
		uint32_t lel_lineInstance;                                      /*!< Line Instance */
		uint32_t lel_callReference;                                     /*!< Call Reference */
		uint32_t lel_payload_rfc_number;                                /*!<  */
		uint32_t lel_payloadType;                                       /*!< payload type */
		uint32_t lel_isConferenceCreator;                               /*!< we can set it to 0 */

		videoParameter_t videoParameter;                                /*!< Video Parameter */
	} OpenMultiMediaChannelMessage;                                         /*!< Open Multi Media Channel Message Structure */

	struct {
		uint32_t lel_conferenceID;                                      /*!< Conference ID */
		uint32_t lel_passThruPartyId;                                   /*!< Pass Through Party ID */
		skinny_codec_t lel_payloadCapability;                     /*!< payload capability */
		uint32_t lel_lineInstance;                                      /*!< Line Instance */
		uint32_t lel_callReference;                                     /*!< Call Reference */
		uint32_t lel_payload_rfc_number;                                /*!<  */
		uint32_t lel_payloadType;                                       /*!< payload type */
		uint32_t lel_isConferenceCreator;                               /*!< we can set it to 0 */

		videoParameter_t videoParameter;                                /*!< Video Parameter */
	} OpenMultiMediaChannelMessage_v17;                                     /*!< Open Multi Media Channel Message Structure */


	/*!
	 * \since 20100104 -MC
	 * \note update 20100722
	 * 0000   ac 00 00 00 00 00 00 00 32 01 00 00 0b 00 00 01
	 0010   82 00 00 01 67 00 00 00 ac 11 01 66 45 15 00 00
	 0020   0b 00 00 01 00 00 00 00 61 00 00 00 88 00 00 00
	 0030   00 0f 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	 0040   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	 0050   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	 0060   00 00 00 00 40 00 00 00 32 00 00 00 5b 52 3a 4c
	 0070   50 20 2d 20 48 50 3a 20 30 2c 20 4e f8 15 24 00
	 0080   c4 02 89 09 9c ee 5a 0a 88 06 18 00 54 ef 5a 0a
	 0090   64 7e fb 77 58 31 f8 77 ff ff ff ff 64 ef 5a 0a
	 00a0   c2 b7 fc 77 78 07 18 00 20 16 24 00 40 16 24 00
	 00b0   20 16 24 00
	 * 
	 * 
	 */
#if 0
	struct {
		uint32_t lel_conferenceID;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		skinny_codec_t lel_payloadCapability;				/*!< payload capability */

		uint32_t bel_remoteIpAddr;					/*!< This field is apparently in big-endian
										   format, even though most other fields are
										   little-endian. */
		uint32_t lel_remotePortNumber;					/*!< Remote Port Number */
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t lel_payload_rfc_number;				/*!< Payload RFC Number */
		uint32_t lel_payloadType;					/*!< payload type */
		uint32_t lel_DSCPValue;						/*!< DSCP Value */

		audioParameter_t audioParameter;				/*!< Audio Parameter */
		videoParameter_t videoParameter;				/*!< Video Parameter */
		dataParameter_t dataParameter;					/*!< Data Parameter */

		uint32_t unknown[12];						/*!< Unknown */

	} StartMultiMediaTransmission;						/*!< Start MultiMedia Transmission Message Structure */

	struct {
		uint32_t lel_conferenceID;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		skinny_codec_t lel_payloadCapability;				/*!< payload capability */
		uint32_t unknown1;						/*!<  */

		char bel_remoteIpAddr[16];					/*!< This field is apparently in big-endian
										   format, even though most other fields are
										   little-endian. */
		uint32_t lel_remotePortNumber;					/*!< Remote Port Number */
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t lel_payload_rfc_number;				/*!< Payload RFC Number */
		uint32_t lel_payloadType;					/*!< payload type */
		uint32_t lel_DSCPValue;						/*!< DSCP Value */

		audioParameter_t audioParameter;				/*!< Audio Parameter */
		videoParameter_t videoParameter;				/*!< Video Parameter */
		dataParameter_t dataParameter;					/*!< Data Parameter */

		uint32_t unknown[12];						/*!< Unknown */

	} StartMultiMediaTransmission_v17;					/*!< Start MultiMedia Transmission Message Structure */
#endif

	struct {
		uint32_t lel_conferenceID;                                      /*!< Conference ID */
		uint32_t lel_passThruPartyId;                                   /*!< Pass Through Party ID */
		skinny_codec_t lel_payloadCapability;                     /*!< payload capability */

		uint32_t bel_remoteIpAddr;                                      /*!< This field is apparently in big-endian
										    format, even though most other fields are
										    little-endian. */
		uint32_t lel_remotePortNumber;                                  /*!< Remote Port Number */
		uint32_t lel_callReference;                                     /*!< Call Reference */
		uint32_t lel_payload_rfc_number;                                /*!< Payload RFC Number */
		uint32_t lel_payloadType;                                       /*!< payload type */
		uint32_t lel_DSCPValue;                                         /*!< DSCP Value */

		videoParameter_t videoParameter;                                /*!< Video Parameter */

	} StartMultiMediaTransmission;                                          /*!< Start MultiMedia Transmission Message Structure */

	struct {
		uint32_t lel_conferenceID;                                      /*!< Conference ID */
		uint32_t lel_passThruPartyId;                                   /*!< Pass Through Party ID */
		skinny_codec_t lel_payloadCapability;                     /*!< payload capability */
		uint32_t unknown1;                                              /*!<  */

		char bel_remoteIpAddr[16];                                      /*!< This field is apparently in big-endian
										    format, even though most other fields are
										    little-endian. */
		uint32_t lel_remotePortNumber;                                  /*!< Remote Port Number */
		uint32_t lel_callReference;                                     /*!< Call Reference */
		uint32_t lel_payload_rfc_number;                                /*!< Payload RFC Number */
		uint32_t lel_payloadType;                                       /*!< payload type */
		uint32_t lel_DSCPValue;                                         /*!< DSCP Value */

		videoParameter_t videoParameter;                                        /*!< Data Parameter */

	} StartMultiMediaTransmission_v17;                                      /*!< Start MultiMedia Transmission Message Structure */

	struct {
		uint32_t lel_displayTimeout;					/*!< Display Timeout */
		uint32_t lel_priority;						/*!< Priority */
		char displayMessage[StationMaxDisplayNotifySize];		/*!< Display Message */
	} DisplayPriNotifyMessage;						/*!< Display Priority Notify Message Structure */

	struct {
	} ClearPriNotifyMessage;						/*!< Clear Priority Notify Message Structure */

	struct {
		uint32_t lel_lineInstance;					/*!< Line Instance */
	} ActivateCallPlaneMessage;						/*!< Activate Call Plane Message Structure */

	struct {
	} DeactivateCallPlaneMessage;						/*!< Deactivate Call Plane Message Structure */

	struct {
		uint32_t lel_status;						/*!< Status */
	} UnregisterAckMessage;							/*!< Unregister Ackknowledge Message Structure */

	struct {
		uint32_t lel_lineInstance;					/*!< Line Instance */
		uint32_t lel_callReference;					/*!< Call Reference */
	} BackSpaceReqMessage;							/*!< Back Space Request Message Message Structure */

	struct {
	} RegisterTokenAck;							/*!< Register Token Ackknowledge Message Structure */

	struct {
		uint32_t lel_tokenRejWaitTime;					/*!< Back Off Time */
	} RegisterTokenReject;							/*!< Register Token Reject Message Structure */

	/*!
	 * 
	 * \since 20100103 -MC
	 */
	struct {
		uint32_t lel_conferenceID;					/*!< Conference ID */
		uint32_t lel_passThruPartyId;					/*!< Pass Through Party ID */
		uint32_t lel_callReference;					/*!< Call Reference */
		uint32_t maxBitRate;						/*!< Maximum BitRate */
	} FlowControlCommandMessage;

	struct {
		uint32_t lel_conferenceID;					/*!< Conference ID */
		uint32_t lel_numberOfReservedParticipants;			/*!< Number of Reserved Participants */
		uint32_t lel_resourceTypes;					/*!< Resource Types */
		uint32_t lel_appID;						/*!< Application ID */
		uint8_t lel_appConfID;						/*!< Conference Application ID */
		char lel_unknown[31];						/*!< Unknown */
		char lel_appData[24];						/*!< Application Data */
		uint32_t lel_data_length;					/*!< Application Data Length */
		uint8_t lel__passThruData;					/*!< Pass Through Data */
	} CreateConferenceReqMessage;						/*!< Create Conference Request Message Structure */

	struct {
		uint32_t lel_conferenceID;					/*!< Conference ID */
	} DeleteConferenceReqMessage;						/*!< Delete Conference Request Message Structure */

	/* SCCP Firmware version > 9.1 */
	struct {
		char le_data[2004];						/*!< XML Alarm Message Data */
	} XMLAlarmMessage;							/*!< XML Alarm Message Structure */

	/* SPA */
	struct {
		StationIdentifier sId;						/*!< Station Identifier */
		uint32_t lel_stationIpAddr;					/*!< Station IP Address */
		uint32_t lel_deviceType;					/*!< Device Type as part of SKINNY_DEVICETYPE_* */
		uint32_t maxStreams;						/*!< Max Streams */
	} SPCPRegisterTokenRequest;
	
	struct {
		uint32_t lel_features;
	} SPCPRegisterTokenAck;

	struct {
		uint32_t lel_features;
	} SPCPRegisterTokenReject;

} sccp_data_t;									/*!< SCCP Data Structure */

/*!
 * \brief SCCP MOO Message Structure
 */

typedef struct {
	uint32_t length;							/*!< Message Length */
	uint32_t lel_reserved;							/*!< Reserved Message */
	uint32_t lel_messageId;							/*!< Message ID */
	sccp_data_t msg;							/*!< Message [SCCP Data] */
} sccp_moo_t;									/*!< MOO Message Structure */

/* So in theory, a message should never be bigger than this.
 * If it is, we abort the connection */
#    define SCCP_MAX_PACKET sizeof(sccp_moo_t)

#    include "sccp_softkeys.h"
#    include "sccp_labels.h"

static const uint8_t softkeysmap[] = {
	SKINNY_LBL_REDIAL,
	SKINNY_LBL_NEWCALL,
	SKINNY_LBL_HOLD,
	SKINNY_LBL_TRANSFER,
	SKINNY_LBL_CFWDALL,
	SKINNY_LBL_CFWDBUSY,
	SKINNY_LBL_CFWDNOANSWER,
	SKINNY_LBL_BACKSPACE,
	SKINNY_LBL_ENDCALL,
	SKINNY_LBL_RESUME,
	SKINNY_LBL_ANSWER,
	SKINNY_LBL_INFO,
	SKINNY_LBL_CONFRN,
	SKINNY_LBL_PARK,
	SKINNY_LBL_JOIN,
	SKINNY_LBL_MEETME,
	SKINNY_LBL_PICKUP,
	SKINNY_LBL_GPICKUP,
	SKINNY_LBL_RMLSTC,
	SKINNY_LBL_CALLBACK,
	SKINNY_LBL_BARGE,
	SKINNY_LBL_DND,
	SKINNY_LBL_CONFLIST,
	SKINNY_LBL_SELECT,
	SKINNY_LBL_PRIVATE,
	SKINNY_LBL_TRNSFVM,
	SKINNY_LBL_DIRTRFR,
	SKINNY_LBL_IDIVERT,
	SKINNY_LBL_VIDEO_MODE,
	SKINNY_LBL_INTRCPT,
	SKINNY_LBL_EMPTY,
	SKINNY_LBL_DIAL,
//      SKINNY_LBL_CBARGE,
};										/*!< Soft Keys Map as INT */

/*!
 * \brief Soft Key Modes Structure
 */

typedef struct {
	uint8_t id;								/*!< Soft Key ID */
	uint8_t *ptr;								/*!< Point to next Mode */
	uint8_t count;								/*!< Soft Key Count */
} softkey_modes;								/*!< Soft Key Modes Structure */

#    define KEYMODE_ONHOOK				0
#    define KEYMODE_CONNECTED			1
#    define KEYMODE_ONHOLD				2
#    define KEYMODE_RINGIN				3
#    define KEYMODE_OFFHOOK 			4
#    define KEYMODE_CONNTRANS			5
#    define KEYMODE_DIGITSFOLL			6
#    define KEYMODE_CONNCONF			7
#    define KEYMODE_RINGOUT 			8
#    define KEYMODE_OFFHOOKFEAT 		9
#    define KEYMODE_INUSEHINT			10
#    define KEYMODE_ONHOOKSTEALABLE		11

/*!
 * \brief Skinny KeyMode Structure
 */
static const struct skinny_keymode {
	uint8_t keymode;
	const char *const text;
} skinny_keymodes[] = {
	/* *INDENT-OFF* */
	{KEYMODE_ONHOOK, "On Hook"},
	{KEYMODE_CONNECTED, "Connected"},
	{KEYMODE_ONHOLD, "On Hold"},
	{KEYMODE_RINGIN, "Ringin"},
	{KEYMODE_OFFHOOK, "Off Hook"},
	{KEYMODE_CONNTRANS, "Connected with Transfer"},
	{KEYMODE_DIGITSFOLL, "Digits after dialing first digit "},
	{KEYMODE_CONNCONF, "Connected with Conference"},
	{KEYMODE_RINGOUT, "Ring Out"},
	{KEYMODE_OFFHOOKFEAT, "Off Hook with Features"},
	{KEYMODE_INUSEHINT, "In Use Hint"}, 
	{KEYMODE_ONHOOKSTEALABLE, "On Hook with Stealable Remote Call"},
	/* *INDENT-ON* */
};

static uint8_t skSet_Onhook[] = {
	SKINNY_LBL_REDIAL,
	SKINNY_LBL_NEWCALL,
	SKINNY_LBL_CFWDALL,
	SKINNY_LBL_DND,
//      SKINNY_LBL_CFWDBUSY,
//      SKINNY_LBL_CFWDNOANSWER,
#    ifdef CS_SCCP_PICKUP
	SKINNY_LBL_PICKUP,
	SKINNY_LBL_GPICKUP,
#    endif
//      SKINNY_LBL_CONFLIST,
	
};										/*!< SKINNY SoftKeys Set "Onhook" as INT */

static uint8_t skSet_OnhookStealable[] = {
	SKINNY_LBL_REDIAL,
	SKINNY_LBL_NEWCALL,
	SKINNY_LBL_CFWDALL,
#    ifdef CS_SCCP_PICKUP
	SKINNY_LBL_PICKUP,
	SKINNY_LBL_GPICKUP,
#    endif
	SKINNY_LBL_DND,
	SKINNY_LBL_INTRCPT,
};										/*!< SKINNY SoftKeys Set "Onhook" as INT */

static uint8_t skSet_Connected[] = {
	SKINNY_LBL_HOLD,
	SKINNY_LBL_ENDCALL,
#    ifdef CS_SCCP_PARK
	SKINNY_LBL_PARK,
#    endif
#    ifdef CS_SCCP_DIRTRFR
	SKINNY_LBL_SELECT,
#    endif
	SKINNY_LBL_CFWDALL,
	SKINNY_LBL_CFWDBUSY,
//      SKINNY_LBL_CFWDNOANSWER,
	SKINNY_LBL_IDIVERT,
};										/*!< SKINNY SoftKeys Set "Connected" as INT */

static uint8_t skSet_Onhold[] = {
	SKINNY_LBL_RESUME,
	SKINNY_LBL_ENDCALL,
	SKINNY_LBL_NEWCALL,
	SKINNY_LBL_TRANSFER,
#    ifdef CS_SCCP_CONFERENCE
	SKINNY_LBL_CONFLIST,
	//SKINNY_LBL_CONFRN,
#    endif
#    ifdef CS_SCCP_DIRTRFR
	SKINNY_LBL_SELECT,
	SKINNY_LBL_DIRTRFR,
#    endif
	SKINNY_LBL_IDIVERT,
};										/*!< SKINNY SoftKeys Set "On Hold" as INT */

static uint8_t skSet_Ringin[] = {
	SKINNY_LBL_ANSWER,
	SKINNY_LBL_ENDCALL,
	SKINNY_LBL_TRNSFVM,
	SKINNY_LBL_IDIVERT,
//      SKINNY_LBL_TRANSFER,
//      SKINNY_LBL_DIRTRFR
};										/*!< SKINNY SoftKeys Set "Ring-IN" as INT */

static uint8_t skSet_Offhook[] = {
	SKINNY_LBL_REDIAL,
	SKINNY_LBL_ENDCALL,
	SKINNY_LBL_PRIVATE,
	SKINNY_LBL_CFWDALL,
	SKINNY_LBL_CFWDBUSY,
//      SKINNY_LBL_CFWDNOANSWER,
#    ifdef CS_SCCP_PICKUP
	SKINNY_LBL_PICKUP,
	SKINNY_LBL_GPICKUP,
#    endif
	SKINNY_LBL_MEETME,
	SKINNY_LBL_BARGE,
//      SKINNY_LBL_CBARGE,
};										/*!< SKINNY SoftKeys Set "OffHook" as INT */

static uint8_t skSet_Conntrans[] = {
	SKINNY_LBL_HOLD,
	SKINNY_LBL_ENDCALL,
	SKINNY_LBL_TRANSFER,
#    ifdef CS_SCCP_CONFERENCE
	SKINNY_LBL_CONFRN,
#    endif
#    ifdef CS_SCCP_PARK
	SKINNY_LBL_PARK,
#    endif
#    ifdef CS_SCCP_DIRTRFR
	SKINNY_LBL_SELECT,
	SKINNY_LBL_DIRTRFR,
#    endif
	SKINNY_LBL_CFWDALL,
	SKINNY_LBL_CFWDBUSY,
	SKINNY_LBL_VIDEO_MODE,
//      SKINNY_LBL_CFWDNOANSWER,
};										/*!< SKINNY SoftKeys Set "Connected With Transfer" as INT */

static uint8_t skSet_DigitsFoll[] = {
	SKINNY_LBL_BACKSPACE,
	SKINNY_LBL_ENDCALL,
	SKINNY_LBL_DIAL,
};										/*!< SKINNY SoftKeys Set "Digits after dialing first digit" as INT */

static uint8_t skSet_Connconf[] = {
	SKINNY_LBL_HOLD,
	SKINNY_LBL_ENDCALL,
	SKINNY_LBL_JOIN,
};										/*!< SKINNY SoftKeys Set "Connected with Conference" as INT */

static uint8_t skSet_RingOut[] = {
	SKINNY_LBL_EMPTY,
	SKINNY_LBL_ENDCALL,
	SKINNY_LBL_TRANSFER,
	SKINNY_LBL_CFWDALL,
	SKINNY_LBL_IDIVERT,
//      SKINNY_LBL_CFWDBUSY,
//      SKINNY_LBL_CFWDNOANSWER,
};										/*!< SKINNY SoftKeys Set "Ring-Out" as INT */

static uint8_t skSet_Offhookfeat[] = {
	SKINNY_LBL_REDIAL,
	SKINNY_LBL_ENDCALL,
};										/*!< SKINNY SoftKeys Set "Off Hook with Features" as INT */

// in use hint keyset
static uint8_t skSet_InUseHint[] = {
	SKINNY_LBL_NEWCALL,
//      SKINNY_LBL_ENDCALL,
	SKINNY_LBL_PICKUP,
	SKINNY_LBL_BARGE,
};										/*!< SKINNY SoftKeys Set "Hint In Use" as INT */

/*! 
 */
static const softkey_modes SoftKeyModes[] = {
	/* According to CCM dump:
	 *  OnHook(0), Connected(1), OnHold(2), RingIn(3)
	 *  OffHook(4), ConnectedWithTransfer(5)
	 *  Digitsafterdialingfirstdigit(6), Connected with Conference (7)
	 *  RingOut (8), OffHookWithFeatures (9), InUseHint(10)
	 */
	/* *INDENT-OFF* */
	{KEYMODE_ONHOOK, skSet_Onhook, sizeof(skSet_Onhook) / sizeof(uint8_t)},
	{KEYMODE_CONNECTED, skSet_Connected, sizeof(skSet_Connected) / sizeof(uint8_t)},
	{KEYMODE_ONHOLD, skSet_Onhold, sizeof(skSet_Onhold) / sizeof(uint8_t)},
	{KEYMODE_RINGIN, skSet_Ringin, sizeof(skSet_Ringin) / sizeof(uint8_t)},
	{KEYMODE_OFFHOOK, skSet_Offhook, sizeof(skSet_Offhook) / sizeof(uint8_t)},
	{KEYMODE_CONNTRANS, skSet_Conntrans, sizeof(skSet_Conntrans) / sizeof(uint8_t)},
	{KEYMODE_DIGITSFOLL, skSet_DigitsFoll, sizeof(skSet_DigitsFoll) / sizeof(uint8_t)},
	{KEYMODE_CONNCONF, skSet_Connconf, sizeof(skSet_Connconf) / sizeof(uint8_t)},
	{KEYMODE_RINGOUT, skSet_RingOut, sizeof(skSet_RingOut) / sizeof(uint8_t)},
	{KEYMODE_OFFHOOKFEAT, skSet_Offhookfeat, sizeof(skSet_Offhookfeat) / sizeof(uint8_t)},
	{KEYMODE_INUSEHINT, skSet_InUseHint, sizeof(skSet_InUseHint) / sizeof(uint8_t)},
	{KEYMODE_ONHOOKSTEALABLE, skSet_OnhookStealable, sizeof(skSet_OnhookStealable) / sizeof(uint8_t)},
	/* *INDENT-ON* */
};										/*!< SoftKey Modes Constants */


/*!
 * \brief SCCP Device Protocol Structure
 *
 * Connect Specific CallBack-Functions to Particular SCCP Protocol Versions
 */
typedef struct {
	const char 	*name;
	const uint8_t 	version;
	
	/* protocol callbacks */
	void (*const sendCallInfo) (const sccp_device_t *device, const sccp_channel_t *channel);
	void (*const sendDialedNumber) (const sccp_device_t *device, const sccp_channel_t *channel);
	void (*const sendRegisterAck) (const sccp_device_t *device, uint8_t keepAliveInterval, uint8_t secondaryKeepAlive, char *dateformat);
	void (*const displayPrompt) (const sccp_device_t *device, uint8_t lineInstance, uint8_t callid, uint8_t timeout, const char *message);
	void (*const displayNotify) (const sccp_device_t *device, uint8_t timeout, const char *message);
	void (*const displayPriNotify) (const sccp_device_t *device, uint8_t priority, uint8_t timeout, const char *message);
	void (*const sendCallforwardMessage) (const sccp_device_t *device, const void *linedevice);
	void (*const sendUserToDeviceDataVersionMessage) (const sccp_device_t *device, const void *xmlData, uint8_t priority);
} sccp_deviceProtocol_t;

uint8_t sccp_protocol_getMaxSupportedVersionNumber(int type);
const sccp_deviceProtocol_t *sccp_protocol_getDeviceProtocol(const sccp_device_t *device, int type);

#endif										/* __SCCP_PROTOCOL_H */
