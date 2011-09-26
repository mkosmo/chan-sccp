
/*!
 * \file 	sccp_config.c
 * \brief 	SCCP Config Class
 * \author 	Sergio Chersovani <mlists [at] c-net.it>
 * \note	Reworked, but based on chan_sccp code.
 *        	The original chan_sccp driver that was made by Zozo which itself was derived from the chan_skinny driver.
 *        	Modified by Jan Czmok and Julien Goodwin
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *		See the LICENSE file at the top of the source tree.
 * \note 	To find out more about the reload function see \ref sccp_config_reload
 * \remarks     Only methods directly related to chan-sccp configuration should be stored in this source file.
 *
 * $Date: 2010-11-17 18:10:34 +0100 (Wed, 17 Nov 2010) $
 * $Revision: 2154 $
 */

/*!
 * \file
 * \ref sccp_config
 *
 * \page sccp_config Loading sccp.conf/realtime configuration implementation
 *
* \section sccp_config_reload How was the new cli command "sccp reload" implemented
 *
 * \code
 * sccp_cli.c
 * 	new implementation of cli reload command
 * 		checks if no other reload command is currently running
 * 		starts loading global settings from sccp.conf (sccp_config_general)
 * 		starts loading devices and lines from sccp.conf(sccp_config_readDevicesLines)
 *
 * sccp_config.c
 * 	modified sccp_config_general
 *
 * 	modified sccp_config_readDevicesLines
 * 		sets pendingDelete for
 * 			devices (via sccp_device_pre_reload),
 * 			lines (via sccp_line_pre_reload)
 * 			softkey (via sccp_softkey_pre_reload)
 *
 * 		calls sccp_config_buildDevice as usual
 * 			calls sccp_config_buildDevice as usual
 * 				find device
 * 				or create new device
 * 				parses sccp.conf for device
 *				set defaults for device if necessary using the default from globals using the same parameter name
 * 				set pendingUpdate on device for parameters marked with SCCP_CONFIG_NEEDDEVICERESET (remove pendingDelete)
 * 			calls sccp_config_buildLine as usual
 * 				find line
 * 				or create new line
 * 				parses sccp.conf for line
 *				set defaults for line if necessary using the default from globals using the same parameter name
 *			 	set pendingUpdate on line for parameters marked with SCCP_CONFIG_NEEDDEVICERESET (remove pendingDelete)
 * 			calls sccp_config_softKeySet as usual ***
 * 				find softKeySet
 *				or create new softKeySet
 *			 	parses sccp.conf for softKeySet
 * 				set pendingUpdate on softKetSet for parameters marked with SCCP_CONFIG_NEEDDEVICERESET (remove pendingDelete)
 *
 * 		checks pendingDelete and pendingUpdate for
 *			skip when call in progress
 * 			devices (via sccp_device_post_reload),
 * 				resets GLOB(device) if pendingUpdate
 * 				removes GLOB(devices) with pendingDelete
 * 			lines (via sccp_line_post_reload)
 * 				resets GLOB(lines) if pendingUpdate
 * 				removes GLOB(lines) with pendingDelete
 * 			softkey (via sccp_softkey_post_reload) ***
 * 				resets GLOB(softkeyset) if pendingUpdate ***
 * 				removes GLOB(softkeyset) with pendingDelete ***
 *
 * channel.c
 * 	sccp_channel_endcall ***
 *		reset device if still device->pendingUpdate,line->pendingUpdate or softkeyset->pendingUpdate
 *
 * \endcode
 *
 * lines marked with "***" still need be implemented
 *
 */

#include "config.h"
#include "common.h"

SCCP_FILE_VERSION(__FILE__, "$Revision: 2154 $")

#ifndef offsetof
#    define offsetof(T, F) ((unsigned int)((char *)&((T *)0)->F))
#endif

#define offsize(T, F) sizeof(((T *)0)->F)

#define G_OBJ_REF(x) offsetof(struct sccp_global_vars,x), offsize(struct sccp_global_vars,x)
#define D_OBJ_REF(x) offsetof(struct sccp_device,x), offsize(struct sccp_device,x)
#define L_OBJ_REF(x) offsetof(struct sccp_line,x), offsize(struct sccp_line,x)
#define S_OBJ_REF(x) offsetof(struct softKeySetConfiguration,x), offsize(struct softKeySetConfiguration,x)
#define H_OBJ_REF(x) offsetof(struct sccp_hotline,x), offsize(struct sccp_hotline,x)

/* dyn config */

/*!
 * \brief Enum for Config Option Types
 */
enum SCCPConfigOptionType {
/* *INDENT-OFF* */
	SCCP_CONFIG_DATATYPE_BOOLEAN			= 1 << 0,
	SCCP_CONFIG_DATATYPE_INT			= 1 << 1,
	SCCP_CONFIG_DATATYPE_STRING			= 1 << 2,
	SCCP_CONFIG_DATATYPE_GENERIC			= 1 << 3,
	SCCP_CONFIG_DATATYPE_STRINGPTR			= 1 << 4,	/* pointer */
	SCCP_CONFIG_DATATYPE_CHAR			= 1 << 5,
/* *INDENT-ON* */
};

/*!
 * \brief Enum for Config Option Flags
 */
enum SCCPConfigOptionFlag {
/* *INDENT-OFF* */
	SCCP_CONFIG_FLAG_IGNORE 			= 1 << 0,		/*< ignore parameter */
	SCCP_CONFIG_FLAG_NONE	 			= 1 << 1,		/*< ignore parameter */
	SCCP_CONFIG_FLAG_DEPRECATED			= 1 << 2,		/*< parameter is deprecated and should not be used anymore, warn user and still set variable */
	SCCP_CONFIG_FLAG_OBSOLETE			= 1 << 3,		/*< parameter is now obsolete warn user and skip */
	SCCP_CONFIG_FLAG_CHANGED			= 1 << 4,		/*< parameter implementation has changed, warn user */
	SCCP_CONFIG_FLAG_REQUIRED			= 1 << 5,		/*< parameter is required */
	SCCP_CONFIG_FLAG_GET_DEVICE_DEFAULT		= 1 << 6,		/*< retrieve default value from device */
	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT		= 1 << 7,		/*< retrieve default value from global */
/* *INDENT-ON* */
};


/*!
 * \brief SCCP Config Option Struct
 */
typedef struct SCCPConfigOption {

/* *INDENT-ON* */
	const char *name;							/*!< Configuration Parameter Name */
	const int offset;							/*!< The offset relative to the context structure where the option value is stored. */
	const size_t size;							/*!< Structure size */
	enum SCCPConfigOptionType type;						/*!< Data type */
	enum SCCPConfigOptionFlag flags;					/*!< Data type */
	sccp_configurationchange_t change;					/*!< Does a change of this value needs a device restart */
	const char *defaultValue;						/*!< Default value */
	 sccp_value_changed_t(*converter_f) (void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);	/*!< Conversion function */
	const char *description;						/*!< Configuration description (config file) or warning message for deprecated or obsolete values */
/* *INDENT-OFF* */
} SCCPConfigOption;

//converter function prototypes 
sccp_value_changed_t sccp_config_parse_codec_preferences(void *dest, const size_t size, const char *value, const boolean_t allow, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_allow_codec (void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_disallow_codec (void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_mailbox(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_tos(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_cos(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_amaflags(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_smallint(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_secondaryDialtoneDigits(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_variables(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_group(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_permit(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_deny(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_button(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_permithosts(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_addons(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_privacyFeature(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_earlyrtp(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_dtmfmode(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_mwilamp(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_debug(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_ipaddress(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_port(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_blindtransferindication(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
//sccp_value_changed_t sccp_config_parse_protocolversion(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_callanswerorder(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_regcontext(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);
sccp_value_changed_t sccp_config_parse_context(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment);

/*!
 * \brief List of SCCP Config Options for SCCP Globals
 */
static const SCCPConfigOption sccpGlobalConfigOptions[]={
/* *INDENT-OFF* */
  {"servername", 			G_OBJ_REF(servername), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"Asterisk",	NULL,			" show this name on the device registration"},
  {"keepalive", 			G_OBJ_REF(keepalive), 			SCCP_CONFIG_DATATYPE_INT,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"60",		NULL,			" Phone keep alive message every 60 secs. Used to check the voicemail and keep an open connection between server and phone (nat). "
																													"Don't set any lower than 60 seconds."},
  {"debug", 				G_OBJ_REF(debug), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"core",		sccp_config_parse_debug," console debug level or categories"
																													"examples: debug = 11 | debug = mwi,event,core | debug = all | debug = none or 0"
																													"possible categories: "
																													"core, sccp, hint, rtp, device, line, action, channel, cli, config, feature, feature_button, softkey, indicate, pbx"
																													"socket, mwi, event, adv_feature, conference, buttontemplate, speeddial, codec, realtime, lock, newcode, high, all, none"},
  {"context", 				G_OBJ_REF(context), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"sccp",		sccp_config_parse_context,"pbx dialplan context"},
  {"dateformat", 			G_OBJ_REF(dateformat), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"D.M.Y",	NULL,			"M-D-Y in any order. Use M/D/YA (for 12h format)"},
  {"bindaddr", 				G_OBJ_REF(bindaddr), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"0.0.0.0",	sccp_config_parse_ipaddress,			"replace with the ip address of the asterisk server (RTP important param)"}, 
  {"port", 				G_OBJ_REF(bindaddr),			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"2000",		NULL,			"listen on port 2000 (Skinny, default)"},
  {"disallow", 				G_OBJ_REF(global_preferences),		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"",		sccp_config_parse_disallow_codec,"First disallow all codecs, for example 'all'"},
  {"allow", 				G_OBJ_REF(global_preferences),		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"",		sccp_config_parse_allow_codec,"Allow codecs in order of preference (Multiple lines allowed)"},
  {"deny", 				G_OBJ_REF(ha),	 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"0.0.0.0/0.0.0.0",	sccp_config_parse_deny,	"Deny every address except for the only one allowed. example: '0.0.0.0/0.0.0.0'"},
  {"permit", 				G_OBJ_REF(ha), 				SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"internal",	sccp_config_parse_permit,"Accept class C 192.168.1.0 example '192.168.1.0/255.255.255.0'"
																													"You may have multiple rules for masking traffic."
																													"Rules are processed from the first to the last."
																													"This General rule is valid for all incoming connections. It's the 1st filter."
																													"using 'internal' will allow the 10.0.0.0, 172.16.0.0 and 192.168.1.0 networks"},
  {"quality_over_size",			G_OBJ_REF(prefer_quality_over_size),	SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"true",		NULL,			"When making decisions during codec selections prefer sound quality over packet size (default true)"},
  {"localnet", 				G_OBJ_REF(localaddr), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		" ",		sccp_config_parse_permit,"All RFC 1918 addresses are local networks, example '192.168.1.0/255.255.255.0'"},
  {"externip", 				G_OBJ_REF(externip), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"",		sccp_config_parse_ipaddress,"IP Address that we're going to notify in RTP media stream"},
  {"externhost", 			G_OBJ_REF(externhost), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"",		NULL,			"Hostname (if dynamic) that we're going to notify in RTP media stream"},
  {"externrefresh", 			G_OBJ_REF(externrefresh), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"60",		sccp_config_parse_smallint,"Expire time in seconds for the hostname (dns resolution)"},
  {"firstdigittimeout", 		G_OBJ_REF(firstdigittimeout), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"16",		sccp_config_parse_smallint,"Dialing timeout for the 1st digit "},
  {"digittimeout", 			G_OBJ_REF(digittimeout), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"8",		sccp_config_parse_smallint,"More digits"},
  {"digittimeoutchar", 			G_OBJ_REF(digittimeoutchar), 		SCCP_CONFIG_DATATYPE_CHAR,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"#",		NULL,			"You can force the channel to dial with this char in the dialing state"},
  {"recorddigittimeoutchar", 		G_OBJ_REF(recorddigittimeoutchar), 	SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"false",	NULL,			"You can force the channel to dial with this char in the dialing state"},
  {"simulate_enbloc",	 		G_OBJ_REF(simulate_enbloc), 		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"true",		NULL,			"Use simulated enbloc dialing to speedup connection when dialing while onhook (older phones)"},
  {"autoanswer_ring_time", 		G_OBJ_REF(autoanswer_ring_time),	SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"1",		sccp_config_parse_smallint,"Ringing time in seconds for the autoanswer, the default is 1"},
  {"autoanswer_tone", 			G_OBJ_REF(autoanswer_tone), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"0x32",		sccp_config_parse_smallint,"Autoanswer confirmation tone. For a complete list of tones: grep SKINNY_TONE sccp_protocol.h"
																													"not all the tones can be played in a connected state, so you have to try."},
  {"remotehangup_tone", 		G_OBJ_REF(remotehangup_tone), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"0x32",		sccp_config_parse_smallint,"Passive hangup notification. 0 for none"},
  {"transfer_tone", 			G_OBJ_REF(transfer_tone), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"0",		sccp_config_parse_smallint,"Confirmation tone on transfer. Works only between SCCP devices"},
  {"callwaiting_tone", 			G_OBJ_REF(callwaiting_tone), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"0x2d",		sccp_config_parse_smallint,"Sets to 0 to disable the callwaiting tone"},
  {"musicclass", 			G_OBJ_REF(musicclass), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"default",	NULL,			"Sets the default music on hold class"},
  {"language", 				G_OBJ_REF(language), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"en",		NULL,			"Default language setting"},
#ifdef CS_MANAGER_EVENTS
  {"callevents", 			G_OBJ_REF(callevents), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"on ",		NULL,			"Generate manager events when phone "
																													"Performs events (e.g. hold)"},
#endif
  {"accountcode", 			G_OBJ_REF(accountcode), 		SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"skinny",	NULL,			"Accountcode to ease billing"},
  {"sccp_tos", 				G_OBJ_REF(sccp_tos), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"0x68",		sccp_config_parse_tos,	"Sets the default sccp signaling packets Type of Service (TOS)  (defaults to 0x68 = 01101000 = 104 = DSCP:011010 = AF31)"
																													"Others possible values : [CS?, AF??, EF], [0x??], [lowdelay, throughput, reliability, mincost(solaris)], none"},
  {"sccp_cos", 				G_OBJ_REF(sccp_cos), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"4",		sccp_config_parse_cos,	"sets the default sccp signaling packets Class of Service (COS) (defaults to 4)"},
  {"audio_tos", 			G_OBJ_REF(audio_tos), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"0xB8",		sccp_config_parse_tos,	"sets the default audio/rtp packets Type of Service (TOS)       (defaults to 0xb8 = 10111000 = 184 = DSCP:101110 = EF)"},
  {"audio_cos", 			G_OBJ_REF(audio_cos), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"6",		sccp_config_parse_cos,	"sets the default audio/rtp packets Class of Service (COS)      (defaults to 6)"},
  {"video_tos", 			G_OBJ_REF(video_tos), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"0x88",		sccp_config_parse_tos,	"sets the default video/rtp packets Type of Service (TOS)       (defaults to 0x88 = 10001000 = 136 = DSCP:100010 = AF41)"},
  {"video_cos", 			G_OBJ_REF(video_cos), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"5",		sccp_config_parse_cos,	"sets the default video/rtp packets Class of Service (COS)      (defaults to 5)"},
  {"echocancel", 			G_OBJ_REF(echocancel), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"on",		NULL,			"sets the phone echocancel for all devices"},
  {"silencesuppression", 		G_OBJ_REF(silencesuppression), 		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"off",		NULL,			"sets the silence suppression for all devices"
																													"we don't have to trust the phone ip address, but the ip address of the connection"},
  {"trustphoneip", 			G_OBJ_REF(trustphoneip), 		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"no",		NULL,			"The phone has a ip address. It could be private, so if the phone is behind NAT "},
  {"earlyrtp", 				G_OBJ_REF(earlyrtp), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"progress",	sccp_config_parse_earlyrtp,"valid options: none, offhook, dial, ringout and progress. default is progress."
																													"The audio stream will be open in the progress and connected state by default."},
  {"dnd", 				G_OBJ_REF(dndmode), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"reject",	sccp_config_parse_dnd,	"turn on the dnd softkey for all devices. Valid values are 'off', 'on' (busy signal), 'reject' (busy signal), 'silent' (ringer = silent)"},
  {"private", 				G_OBJ_REF(privacy), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"on",		NULL,			"permit the private function softkey"},
  {"mwilamp", 				G_OBJ_REF(mwilamp), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"on",		sccp_config_parse_mwilamp,"Set the MWI lamp style when MWI active to on, off, wink, flash or blink"},
  {"mwioncall", 			G_OBJ_REF(mwioncall), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"off",		NULL,			"Set the MWI on call."},
  {"blindtransferindication", 		G_OBJ_REF(blindtransferindication),	SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"ring",		sccp_config_parse_blindtransferindication,"moh or ring. the blind transfer should ring the caller or just play music on hold"},
  {"cfwdall", 				G_OBJ_REF(cfwdall), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"on",		NULL,			"activate the callforward ALL stuff and softkeys"},
  {"cfwdbusy", 				G_OBJ_REF(cfwdbusy), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"on",		NULL,			"activate the callforward BUSY stuff and softkeys"},
  {"cfwdnoanswer", 			G_OBJ_REF(cfwdnoanswer), 		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"on",		NULL,			"activate the callforward NOANSWER stuff and softkeys"},
  {"nat", 				G_OBJ_REF(nat), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"off",		NULL,			"Global NAT support (default Off)"},
  {"directrtp", 			G_OBJ_REF(directrtp), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"off",		NULL,			"This option allow devices to do direct RTP sessions (default Off)"},
  {"allowoverlap", 			G_OBJ_REF(useoverlap), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"off ",		NULL,			"Enable overlap dialing support. If enabled, starts dialing immediately and sends remaing digits as DTMF/inband."
																													"Use with extreme caution as it is very dialplan and provider dependent. (Default is off)"},
  {"callgroup", 			G_OBJ_REF(callgroup), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"",		sccp_config_parse_group,"We are in caller groups 1,3,4. Valid for all lines"},
#ifdef CS_SCCP_PICKUP
  {"pickupgroup", 			G_OBJ_REF(pickupgroup), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"",		sccp_config_parse_group,"We can do call pick-p for call group 1,3,4,5. Valid for all lines"},
  {"pickupmodeanswer", 			G_OBJ_REF(pickupmodeanswer), 		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"",		NULL,			"We can do call pick-p for call group 1,3,4,5. Valid for all lines"},
#endif
  {"amaflags", 				G_OBJ_REF(amaflags), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"",		sccp_config_parse_amaflags,"Sets the default AMA flag code stored in the CDR record"},
//  {"protocolversion", 			G_OBJ_REF(protocolversion), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_OBSOLETE,		SCCP_CONFIG_NOUPDATENEEDED,		"17",		sccp_config_parse_protocolversion,"skinny version protocol. Just for testing. 1 to 17 (excluding 12-14)"},
  {"protocolversion", 			G_OBJ_REF(protocolversion), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_OBSOLETE,		SCCP_CONFIG_NOUPDATENEEDED,		"20",		NULL,"skinny version protocol. Just for testing. 1 to 17 (excluding 12-14)"},
  {"callanswerorder", 			G_OBJ_REF(callanswerorder), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"oldestfirst",	sccp_config_parse_callanswerorder,"oldestfirst or lastestfirst"},
  {"regcontext", 			G_OBJ_REF(regcontext), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"sccpregistration",sccp_config_parse_regcontext,	"SCCP Lines will we added to this context in asterisk for Dundi lookup purposes. "
																													"Don not set to a manually created context. The context will be autocreated. You can share the sip/iax context if you like."},
#ifdef CS_SCCP_REALTIME
  {"devicetable", 			G_OBJ_REF(realtimedevicetable), 	SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"sccpdevice",	NULL,			"datebasetable for devices"},
  {"linetable", 			G_OBJ_REF(realtimelinetable), 		SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"sccpline",	NULL,			"datebasetable for lines"},
#endif
  {"meetme", 				G_OBJ_REF(meetme), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"on",		NULL,			"enable/disable conferencing via meetme (on/off), make sure you have one of the meetme apps mentioned below activated in module.conf"
																													"when switching meetme=on it will search for the first of these three possible meetme applications and set these defaults"
																													"{'MeetMe', 'qd'},"
																													"{'ConfBridge', 'Mac'},"
																													"{'Konference', 'MTV'}"},
  {"meetmeopts", 			G_OBJ_REF(meetmeopts), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"qxd",		NULL,			" options to send the meetme application, defaults are dependent on meetme app see the list above"
																													"Other options (app_meetme: A,a,b,c,C,d,D,E,e,F,i,I,l,L,m,M,o,p,P,q,r,s,S,t,T,w,x,X,1) see meetme specific documentation"},

/* \todo need a solution to set boolean flags in a bit array */
/*
#if ASTERISK_VERSION_NUMBER >= 10400
	// handle jb in configuration just let asterisk do that
	if (!pbx_jb_read_conf(&GLOB(global_jbconf), v->name, v->value)) {
		// Found a jb parameter
		continue;
	}
#endif
*/

//  {"jbenable", 				G_OBJ_REF(global_jbconf.flags) /*<<0*/, SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"no",		NULL,			" Enables the use of a jitterbuffer on the receiving side of a"
//																													"sccp channel. Defaults to 'no'. An enabled jitterbuffer will"
//																													"be used only if the sending side can create and the receiving"
//																													"side can not accept jitter. The sccp channel can accept"
//																													"jitter, thus a jitterbuffer on the receive sccp side will be"
//																													"used only if it is forced and enabled."},
//  {"jbforce", 				G_OBJ_REF(global_jbconf.flags) /*<<1*/,	SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"no",		NULL,			"Forces the use of a jitterbuffer on the receive side of a sccp"
//																													"channel. Defaults to 'no'."},
//  {"jbmaxsize", 			G_OBJ_REF(global_jbconf.max_size),	SCCP_CONFIG_DATATYPE_INT,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"200",		NULL,			"Max length of the jitterbuffer in milliseconds."},
//  {"jbresyncthreshold", 		G_OBJ_REF(global_jbconf.resync_threshold),SCCP_CONFIG_DATATYPE_INT,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"1000",		NULL,			"Jump in the frame timestamps over which the jitterbuffer is"
//																													"resynchronized. Useful to improve the quality of the voice, with"
//																													"big jumps in/broken timestamps, usually sent from exotic devices"
//																													"and programs. Defaults to 1000."},
//  {"jbimpl", 				G_OBJ_REF(global_jbconf.impl),		SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"fixed",	NULL,			"Jitterbuffer implementation, used on the receiving side of a"
//																													"sccp channel. Two implementations are currently available"
//																													"- 'fixed' (with size always equals to jbmaxsize)"
//																													"- 'adaptive' (with variable size, actually the new jb of IAX2)."
//																													"Defaults to fixed."},
//  {"jblog", 				G_OBJ_REF(global_jbconf.flags) /*<<2*/,	SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"no",		NULL,			" Enables jitterbuffer frame logging. Defaults to 'no'."},
//

  {"hotline_enabled", 			G_OBJ_REF(allowAnonymous), 		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"no",		NULL,			" Setting the hotline Feature on a device, will make it connect to a predefined extension as soon as the Receiver"
																													"is picked up or the 'New Call' Button is pressed. No number has to be given. This works even on devices which "
																													"have no entry in the config file or realtime database. "
																													"The hotline function can be used in different circumstances, for example at a door, where you want people to be "
																													"able to only call one number, or for unprovisioned phones to only be able to call the helpdesk to get their phone"
																													"set up	If hotline_enabled = yes, any device which is not included in the configuration explicitly will be allowed "
																													"to registered as a guest device. All such devices will register on a single shared line called 'hotline'."},

//  {"hotline_context",			(offsetof(struct sccp_global_vars,hotline) + offsetof(struct sccp_hotline,line) + offsetof(struct sccp_line,context)), offsize(struct sccp_line,context), 	SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"sccp",	NULL,			""},
//  {"hotline_extension", 		(offsetof(struct sccp_global_vars,hotline) + offsetof(struct sccp_hotline,exten)), offsize(struct sccp_hotline,exten),		SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"111",		NULL,			""},
  {"fallback",				G_OBJ_REF(token_fallback),		SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"false",	NULL, 			"Immediately fallback to primairy/master server when it becomes available (master/slave asterisk cluster) (TokenRequest)"
																													"Possible values are: true/false/odd/even (odd/even uses the last digit of the MAC address to make the decision)"
																													"Value can be changed online via CLI/AMI command \"fallback=[true/false]\""},
  {"backoff_time", 			G_OBJ_REF(token_backoff_time),		SCCP_CONFIG_DATATYPE_INT,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"60",		NULL,			"Time to wait before re-asking to fallback to primairy server (Token Reject Backoff Time)"},
/* *INDENT-ON* */
};

/*!
 * \brief List of SCCP Config Options for SCCP Devices
 */
static const SCCPConfigOption sccpDeviceConfigOptions[] = {
/* *INDENT-OFF* */
  {"name", 				0, 				0,	SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_IGNORE,		SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"device name"},
  {"type", 				0, 				0,	SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_IGNORE,		SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"type name"},
  {"device", 				D_OBJ_REF(config_type),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		NULL,	NULL, 				"device type"},
  {"devicetype", 			D_OBJ_REF(config_type),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		NULL,	NULL, 				"device type"},
  {"type", 				D_OBJ_REF(config_type),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		NULL,	NULL, 				"used for device templates, value will be inherited."},
  {"description", 			D_OBJ_REF(description),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		NULL,	NULL, 				"device description"},
  {"keepalive", 			D_OBJ_REF(keepalive), 			SCCP_CONFIG_DATATYPE_INT,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NEEDDEVICERESET,		NULL,	NULL, 				"set keepalive to 60"},
  {"tzoffset", 				D_OBJ_REF(tz_offset), 			SCCP_CONFIG_DATATYPE_INT,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		"0",	NULL, 				"time zone offset"},
  {"disallow", 				D_OBJ_REF(preferences), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	sccp_config_parse_disallow_codec,""},
  {"allow", 				D_OBJ_REF(preferences), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	sccp_config_parse_allow_codec,""},
  {"transfer", 				D_OBJ_REF(transfer),			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"on",	NULL, 				"enable or disable the transfer capability. It does remove the transfer softkey"},
  {"park", 				D_OBJ_REF(park),			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"on",	NULL, 				"take a look to the compile how-to. Park stuff is not compiled by default"},
  {"cfwdall", 				D_OBJ_REF(cfwdall), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		"off",	NULL, 				"activate the call forward stuff and soft keys"},
  {"cfwdbusy", 				D_OBJ_REF(cfwdbusy), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		"off",	NULL, 				"allow call forward when line is busy"},
  {"cfwdnoanswer", 			D_OBJ_REF(cfwdnoanswer),		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		"off",	NULL, 				"allow call forward when line if not being answered"},
  {"dnd",	 			D_OBJ_REF(dndFeature.enabled),		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_OBSOLETE,		SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"parameter 'dnd' is obsolete. This setting has moved to the line definition. To allow/disallow dnd you should use dndFeature in device and setup dnd per line."},
  {"dndFeature",	 		D_OBJ_REF(dndFeature.enabled),		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"on",	NULL, 				"allow usage do not disturb button"},
  {"dtmfmode", 				D_OBJ_REF(dtmfmode), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		"inband",sccp_config_parse_dtmfmode,	"inband or outofband. outofband is the native cisco dtmf tone play."
                                                                                                                                                                                                                                        "Some phone model does not play dtmf tones while connected (bug?), so the default is inband"},
  {"imageversion", 			D_OBJ_REF(imageversion), 		SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NEEDDEVICERESET,		NULL,	NULL,				"useful to upgrade old firmwares (the ones that do not load *.xml from the tftp server)"},
  {"deny", 				D_OBJ_REF(ha),	 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NEEDDEVICERESET,		NULL,	sccp_config_parse_deny,		"Same as general"},
  {"permit", 				D_OBJ_REF(ha), 				SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NEEDDEVICERESET,		NULL,	sccp_config_parse_permit,	"This device can register only using this ip address"},
  {"audio_tos", 			D_OBJ_REF(audio_tos),			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_DEVICE_DEFAULT, 	SCCP_CONFIG_NEEDDEVICERESET, 		NULL,	sccp_config_parse_tos, 		"sets the audio/rtp packets Type of Service (TOS)  (defaults to 0xb8 = 10111000 = 184 = DSCP:101110 = EF)"
                                                                                                                                                                                                                                        "Others possible values : 0x??, lowdelay, throughput, reliability, mincost(solaris), none"},
  {"audio_cos", 			D_OBJ_REF(audio_cos),			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_DEVICE_DEFAULT, 	SCCP_CONFIG_NEEDDEVICERESET, 		NULL,	sccp_config_parse_cos, 		"sets the audio/rtp packets Class of Service (COS) (defaults to 6)"},
  {"video_tos", 			D_OBJ_REF(video_tos),			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_DEVICE_DEFAULT, 	SCCP_CONFIG_NEEDDEVICERESET,		NULL, 	sccp_config_parse_tos, 		"sets the video/rtp packets Type of Service (TOS)  (defaults to 0x88 = 10001000 = 136 = DSCP:100010 = AF41)"},
  {"video_cos", 			D_OBJ_REF(video_cos),			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_DEVICE_DEFAULT, 	SCCP_CONFIG_NEEDDEVICERESET, 		NULL,	sccp_config_parse_cos, 		"sets the video/rtp packets Class of Service (COS) (defaults to 5)"},
  {"trustphoneip", 			D_OBJ_REF(trustphoneip), 		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NEEDDEVICERESET,		NULL,	NULL, 				"The phone has a ip address. It could be private, so if the phone is behind NAT "
                                                                                                                                                                                                                                        "we don't have to trust the phone ip address, but the ip address of the connection"},
  {"nat", 				D_OBJ_REF(nat), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_DEPRECATED | SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"Device NAT support (default Off)"},
  {"directrtp", 			D_OBJ_REF(directrtp), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"This option allow devices to do direct RTP sessions (default Off)								"},
  {"earlyrtp", 				D_OBJ_REF(earlyrtp), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	sccp_config_parse_earlyrtp, 	"valid options: none, offhook, dial, ringout and progress. default is progress."
                                                                                                                                                                                                                                        "The audio stream will be open in the progress and connected state by default."},
  {"private", 				D_OBJ_REF(privacyFeature.enabled), 	SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"permit the private function softkey for this device"},
  {"privacy", 				D_OBJ_REF(privacyFeature),	 	SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	sccp_config_parse_privacyFeature,"permit the private function softkey for this device"},
  {"mwilamp",				D_OBJ_REF(mwilamp), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	sccp_config_parse_mwilamp, 	"Set the MWI lamp style when MWI active to on, off, wink, flash or blink"},
  {"mwioncall", 			D_OBJ_REF(mwioncall), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"Set the MWI on call."},
  {"meetme", 				D_OBJ_REF(meetme), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"enable/disable conferencing via app_meetme (on/off)"},
  {"meetmeopts", 			D_OBJ_REF(meetmeopts), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"options to send the app_meetme application (default 'qd' = quiet,dynamic pin)"
                                                                                                                                                                                                                                        "Other options (A,a,b,c,C,d,D,E,e,F,i,I,l,L,m,M,o,p,P,q,r,s,S,t,T,w,x,X,1) see app_meetme documentation"},
  {"softkeyset", 			D_OBJ_REF(softkeyDefinition),		SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		NULL,	NULL, 			"use specified softkeyset with name softkeyset1"},
#ifdef CS_ADV_FEATURES
  {"useRedialMenu", 			D_OBJ_REF(useRedialMenu), 		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"off",	NULL, 				"show the redial phone book list instead of dialing the last number (adv_feature)"},
#endif
#ifdef CS_SCCP_PICKUP
  {"pickupexten", 			D_OBJ_REF(pickupexten), 		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"off",	NULL, 				"enable Pickup function to direct pickup an extension"},
  {"pickupcontext", 			D_OBJ_REF(pickupcontext), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"sccp",	sccp_config_parse_context, 				"context where direct pickup search for extensions. if not set it will be ignored."},
  {"pickupmodeanswer", 			D_OBJ_REF(pickupmodeanswer), 		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"on",	NULL, 				"on = asterisk way, the call has been answered when picked up"},
#endif
  {"monitor", 				D_OBJ_REF(monitorFeature.enabled), 	SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				""},
  {"allowoverlap", 			D_OBJ_REF(overlapFeature.enabled), 	SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				""},
  {"setvar", 				D_OBJ_REF(variables),			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,		 	SCCP_CONFIG_NOUPDATENEEDED, 		NULL, 	sccp_config_parse_variables,	"extra variables to be set on line initialization multiple entries possible (for example the sip number to use when dialing outside)"
                                                                                                                                                                                                                                        "format setvar=param=value, for example setvar=sipno=12345678"},
  {"permithost", 			D_OBJ_REF(permithosts), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		NULL,	sccp_config_parse_permithosts,	""},
  {"addon", 				D_OBJ_REF(addons),	 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		NULL,	sccp_config_parse_addons,	""},
  {"dtmfmode", 				D_OBJ_REF(dtmfmode),	 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	sccp_config_parse_dtmfmode,	""},
  {"button", 				D_OBJ_REF(buttonconfig), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NEEDDEVICERESET,		NULL,	sccp_config_parse_button,	""},
  {"digittimeout", 			D_OBJ_REF(digittimeout), 		SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		"8",	sccp_config_parse_smallint,	"More digits"},
/* *INDENT-ON* */
};

/*!
 * \brief List of SCCP Config Options for SCCP Lines
 */
static const SCCPConfigOption sccpLineConfigOptions[] = {
/* *INDENT-OFF* */
  {"name", 				0, 	0, 				SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_IGNORE,		SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"line name"},
  {"line", 				0,	0, 				SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_IGNORE,		SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"line name"},
  {"type", 				0, 	0, 				SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_IGNORE,		SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"line"},
  {"id", 				L_OBJ_REF(id),				SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"id"},
  {"pin", 				L_OBJ_REF(pin), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_REQUIRED,		SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"pin"},
  {"label", 				L_OBJ_REF(label), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_REQUIRED, 		SCCP_CONFIG_NEEDDEVICERESET,		NULL,	NULL, 				"label"},
  {"description", 			L_OBJ_REF(description),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"description"},
  {"context", 				L_OBJ_REF(context), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"pbx dialing context"},
  {"cid_name", 				L_OBJ_REF(cid_name), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_REQUIRED,		SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"callerid name"},
  {"cid_num", 				L_OBJ_REF(cid_num), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_REQUIRED,		SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"callerid number"},
  {"defaultSubscriptionId_name",	L_OBJ_REF(defaultSubscriptionId.name), 	SCCP_CONFIG_DATATYPE_STRING, 	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"Name used on a shared line when no name is specified on the line button for the device"},
  {"defaultSubscriptionId_number",	L_OBJ_REF(defaultSubscriptionId.number),SCCP_CONFIG_DATATYPE_STRING, 	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"Number used on a shared line when no name is specified on the line button for the device"},
  {"callerid", 				0, 	0, 				SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_OBSOLETE, 		SCCP_CONFIG_NOUPDATENEEDED, 		NULL, 	NULL, 				"obsolete callerid param. Use cid_num and cid_name"},
  {"mailbox", 				L_OBJ_REF(mailboxes), 			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,	 		SCCP_CONFIG_NOUPDATENEEDED, 		NULL, 	sccp_config_parse_mailbox, 	"Mailbox to store messages in"},
  {"vmnum", 				L_OBJ_REF(vmnum), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"Number to dial to get to the users Mailbox"},
  {"adhocNumber", 			L_OBJ_REF(adhocNumber), 		SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"Adhoc Number or Private-line automatic ring down (PLAR):"
                                                                                                                                                                                                                                        "Adhoc/PLAR circuits have statically configured endpoints and do not require the user dialing to connect calls."
                                                                                                                                                                                                                                        " - The adhocNumber is dialed as soon as the Phone is taken off-hook or when the new-call button is pressed."
                                                                                                                                                                                                                                        " - The number will not be dialed when choosing a line; so when you choose a line you can enter a number manually."},
  {"meetme", 				L_OBJ_REF(meetme), 			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_DEVICE_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"enable/disable conferencing via meetme, make sure you have one of the meetme apps mentioned below activated in module.conf."
                                                                                                                                                                                                                                        "When switching meetme=on it will search for the first of these three possible meetme applications and set these defaults."
                                                                                                                                                                                                                                        "Meetme=>'qd', ConfBridge=>'Mac', Konference=>'MTV'"},
  {"meetmenum",				L_OBJ_REF(meetmenum), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"This extension will receive meetme requests, SCCP_MEETME_ROOM channel variable will"
					                                                                                                                                                                                                "contain the room number dialed into simpleswitch (this parameter is going to be removed)."},
  {"meetmeopts", 			L_OBJ_REF(meetmeopts),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_GET_DEVICE_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"options to send the meetme application, defaults are dependent on meetme app see the list above."
                                                                                                                                                                                                                                        "Other options (app_meetme: A,a,b,c,C,d,D,E,e,F,i,I,l,L,m,M,o,p,P,q,r,s,S,t,T,w,x,X,1) see conferencing app for specific documentation"},
  {"transfer", 				L_OBJ_REF(transfer),			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_DEVICE_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"per line transfer capability"},
  {"incominglimit", 			L_OBJ_REF(incominglimit),		SCCP_CONFIG_DATATYPE_INT,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"allow x number of incoming calls (call waiting)"},
  {"echocancel", 			L_OBJ_REF(echocancel),			SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"sets the phone echocancel for this line"},
  {"silencesuppression",		L_OBJ_REF(silencesuppression),		SCCP_CONFIG_DATATYPE_BOOLEAN,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"sets the silence suppression for this line"},
  {"language", 				L_OBJ_REF(language),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"sets the language setting per line"},
  {"musicclass", 			L_OBJ_REF(musicclass),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT,	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"sets the music on hold class per line"},
  {"accountcode", 			L_OBJ_REF(accountcode),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"accountcode for this line to make billing per call possible"},
  {"amaflags", 				L_OBJ_REF(amaflags),			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,		 	SCCP_CONFIG_NOUPDATENEEDED,	 	NULL, 	sccp_config_parse_amaflags, 	"sets the AMA flags stored in the CDR record for this line"},
  {"callgroup", 			L_OBJ_REF(callgroup),			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,		 	SCCP_CONFIG_NOUPDATENEEDED,		NULL,	sccp_config_parse_group,	"sets the caller groups this line is a member of"},
  {"pickupgroup", 			L_OBJ_REF(pickupgroup),			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,		 	SCCP_CONFIG_NOUPDATENEEDED,		NULL, 	sccp_config_parse_group,	"sets the pickup groups this line is a member of (this phone can pickup calls from remote phones which are in this caller group"},
  {"trnsfvm", 				L_OBJ_REF(trnsfvm),			SCCP_CONFIG_DATATYPE_STRINGPTR,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"extension to redirect the caller to for voice mail"},
  {"secondary_dialtone_digits", 	L_OBJ_REF(secondary_dialtone_digits),	SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,	 	"9", 	sccp_config_parse_secondaryDialtoneDigits, 	"digits to indicate an external line to user (secondary dialtone) (max 9 digits)"},
  {"secondary_dialtone_tone", 		L_OBJ_REF(secondary_dialtone_tone),	SCCP_CONFIG_DATATYPE_INT, 	SCCP_CONFIG_FLAG_NONE, 			SCCP_CONFIG_NOUPDATENEEDED, 		"0x22", NULL,			 	"outside dialtone frequency"},
  {"setvar", 				L_OBJ_REF(variables),			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_NONE,		 	SCCP_CONFIG_NOUPDATENEEDED, 		NULL, 	sccp_config_parse_variables,	"extra variables to be set on line initialization multiple entries possible (for example the sip number to use when dialing outside)"
                                                                                                                                                                                                                                        "format setvar=param=value, for example setvar=sipno=12345678"},
  {"dnd", 				L_OBJ_REF(dndmode),			SCCP_CONFIG_DATATYPE_GENERIC,	SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT, 	SCCP_CONFIG_NOUPDATENEEDED, 		"reject", sccp_config_parse_dnd, 		"allow setting dnd for this line. Valid values are 'off', 'on' (busy signal), 'reject' (busy signal), 'silent' (ringer = silent) or user to toggle on phone"},
  {"regexten", 				L_OBJ_REF(regexten),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"SCCP Lines will we added to the regcontext with this number for Dundi look up purpose"
                                                                                                                                                                                                                                        "If regexten is not filled in the line name (categoryname between []) will be used"},
/* begin deprecated / obsolete test data */
  {"test1", 				L_OBJ_REF(regexten),			SCCP_CONFIG_DATATYPE_STRING, 	SCCP_CONFIG_FLAG_OBSOLETE,		SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"Parameter 'test' is now obsolete, please use parameter 'test3', see CHANGES and or documentation"},
  {"test2", 				L_OBJ_REF(regexten),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_DEPRECATED,		SCCP_CONFIG_NOUPDATENEEDED,		NULL,	NULL, 				"Parameter 'test' is now deprecated, please use parameter 'test3' in the future, see CHANGES and or documentation"},
/* end test data */
/* *INDENT-ON* */
};

/*!
 * \brief List of SCCP Config Options for SCCP SoftKey
 */
static const SCCPConfigOption sccpSoftKeyConfigOptions[] = {
  {"type",				0,	0, 				SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_IGNORE,		SCCP_CONFIG_NOUPDATENEEDED,		"softkeyset",								NULL, 				""},
  {"name", 				S_OBJ_REF(name),			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		NULL,									NULL, 				"softkeyset name"},
  {"connected",				S_OBJ_REF(modes[0]), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"hold,endcall,park,select,cfwdall,cfwdbusy,idivert",			NULL, 				""},
  {"onhold",				S_OBJ_REF(modes[1]), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"resume,newcall,endcall,transfer,conflist,select,dirtrfr,idivert,meetme",NULL, 				""},
  {"ringin",				S_OBJ_REF(modes[2]), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"answer,endcall,transvm,idivert",					NULL, 				""},
  {"offhook",				S_OBJ_REF(modes[3]), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"redial,endcall,private,cfwdall,cfwdbusy,pickup,gpickup,meetme,barge",	NULL, 				""},
  {"conntrans",				S_OBJ_REF(modes[4]), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"hold,endcall,transfer,conf,park,select,dirtrfr,meetme,cfwdall,cfwdbusy",NULL, 				""},
  {"digitsfoll",			S_OBJ_REF(modes[5]), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"back,endcall",								NULL, 				""},
  {"connconf",				S_OBJ_REF(modes[6]), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"conflist,endcall,join,hold",						NULL, 				""},
  {"ringout",				S_OBJ_REF(modes[7]), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"endcall,transfer,cfwdall,idivert",					NULL, 				""},
  {"offhookfeat",			S_OBJ_REF(modes[8]), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"redial,endcall",							NULL, 				""},
  {"onhint",				S_OBJ_REF(modes[9]), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"newcall,pickup,barge",							NULL, 				""},
  {"onstealable",			S_OBJ_REF(modes[10]), 			SCCP_CONFIG_DATATYPE_STRING,	SCCP_CONFIG_FLAG_NONE,			SCCP_CONFIG_NOUPDATENEEDED,		"redial,newcall,cfwdall,pickup,gpickup,dnd,intrcpt",			NULL, 				""},
};

/*!
 * \brief SCCP Config Option Struct
 */
typedef struct SCCPConfigSegment {
	const char *name;
	const sccp_config_segment_t segment;
	const SCCPConfigOption *config;
	long unsigned int config_size;
} SCCPConfigSegment;

/*!
 * \brief SCCP Config Option Struct Initialization
 */
static const SCCPConfigSegment sccpConfigSegments[] = {
	{"global", SCCP_CONFIG_GLOBAL_SEGMENT, sccpGlobalConfigOptions, ARRAY_LEN(sccpGlobalConfigOptions)},
	{"device", SCCP_CONFIG_DEVICE_SEGMENT, sccpDeviceConfigOptions, ARRAY_LEN(sccpDeviceConfigOptions)},
	{"line", SCCP_CONFIG_LINE_SEGMENT, sccpLineConfigOptions, ARRAY_LEN(sccpLineConfigOptions)},
	{"softkey", SCCP_CONFIG_SOFTKEY_SEGMENT, sccpSoftKeyConfigOptions, ARRAY_LEN(sccpSoftKeyConfigOptions)},
};

/*!
 * \brief Find of SCCP Config Options
 */
//static const SCCPConfigOption *sccp_find_segment(const sccp_config_segment_t segment)
static const SCCPConfigSegment *sccp_find_segment(const sccp_config_segment_t segment)
{
	uint8_t i = 0;

	for (i = 0; i < ARRAY_LEN(sccpConfigSegments); i++) {
		if (sccpConfigSegments[i].segment == segment)
			return &sccpConfigSegments[i];
	}
	return NULL;
}

/*!
 * \brief Find of SCCP Config Options
 */
static const SCCPConfigOption *sccp_find_config(const sccp_config_segment_t segment, const char *name)
{
	long unsigned int i = 0;
	const SCCPConfigSegment *sccpConfigSegment = sccp_find_segment(segment);
	const SCCPConfigOption *config = sccpConfigSegment->config;

	for (i = 0; i < sccpConfigSegment->config_size; i++) {
		if (!strcasecmp(config[i].name, name))
			return &config[i];
	}

	return NULL;
}

/*!
 * \brief Parse SCCP Config Option Value
 *
 * \todo add errormsg return to sccpConfigOption->converter_f: so we can have a fixed format the returned errors to the user
 */
static sccp_configurationchange_t sccp_config_object_setValue(void *obj, const char *name, const char *value, uint8_t lineno, const sccp_config_segment_t segment)
{
	const SCCPConfigSegment *sccpConfigSegment = sccp_find_segment(segment);
	const SCCPConfigOption *sccpConfigOption = sccp_find_config(segment, name);
	void *dst;
	int type;								/* enum wrapper */
	int flags;								/* enum wrapper */

	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;		/* indicates config value is changed or not */
	sccp_configurationchange_t changes = SCCP_CONFIG_NOUPDATENEEDED;

	int intnum;
	boolean_t bool;
	char *str;
	char oldChar;

	if (!sccpConfigOption) {
		pbx_log(LOG_WARNING, "Unknown param at %s:%d:%s='%s'\n", sccpConfigSegment->name, lineno, name, value);
		return SCCP_CONFIG_NOUPDATENEEDED;
	}

	if (sccpConfigOption->offset <= 0)
		return SCCP_CONFIG_NOUPDATENEEDED;

	dst = ((uint8_t *) obj) + sccpConfigOption->offset;
	type = sccpConfigOption->type;
	flags = sccpConfigOption->flags;

	switch (flags) {
	case SCCP_CONFIG_FLAG_IGNORE:
		sccp_log(DEBUGCAT_CORE) (VERBOSE_PREFIX_2 "config parameter %s='%s' in line %d ignored\n", name, value, lineno);
		return SCCP_CONFIG_NOUPDATENEEDED;
		
	case SCCP_CONFIG_FLAG_CHANGED:
		pbx_log(LOG_NOTICE, "changed config param at %s='%s' in line %d\n - %s -> please check sccp.conf file\n", name, value, lineno, sccpConfigOption->description);
		
	case SCCP_CONFIG_FLAG_DEPRECATED:
		pbx_log(LOG_NOTICE, "deprecated config param at %s='%s' in line %d\n - %s -> using old implementation\n", name, value, lineno, sccpConfigOption->description);
		
	case SCCP_CONFIG_FLAG_OBSOLETE:
		pbx_log(LOG_WARNING, "obsolete config param at %s='%s' in line %d\n - %s -> param skipped\n", name, value, lineno, sccpConfigOption->description);
		return SCCP_CONFIG_NOUPDATENEEDED;
		
	case SCCP_CONFIG_FLAG_REQUIRED:
		if (NULL == value) {
			pbx_log(LOG_WARNING, "required config param at %s='%s' - %s\n", name, value, sccpConfigOption->description);
			return SCCP_CONFIG_CHANGE_INVALIDVALUE;
		}
	}

	/* warn user that value is being overwritten */
	switch (type) {
	  
	case SCCP_CONFIG_DATATYPE_CHAR:
		oldChar = *(char *)dst;

		if (!sccp_strlen_zero(value)) {
			if ( oldChar != value[0] ) {
				changes = SCCP_CONFIG_CHANGE_CHANGED;
				*(char *)dst = value[0];
			}
		}
		break;  
	
	case SCCP_CONFIG_DATATYPE_STRING:
		str = (char *)dst;

		if (strcasecmp(str, value) ) {
			sccp_log(DEBUGCAT_CORE) (VERBOSE_PREFIX_2 "config parameter %s '%s' != '%s'\n", name, str, value);
			changes = SCCP_CONFIG_CHANGE_CHANGED;
			pbx_copy_string(dst, value, sccpConfigOption->size);
//		}else{
//			sccp_log(DEBUGCAT_CORE) (VERBOSE_PREFIX_2 "config parameter %s '%s' == '%s'\n", name, str, value);
		}
		break;

	case SCCP_CONFIG_DATATYPE_STRINGPTR:
		changed = SCCP_CONFIG_CHANGE_NOCHANGE;
		str = *(void **)dst;

		if (!sccp_strlen_zero(value)) {
			if (str) {
				if (strcasecmp(str, value)) {
					changed = SCCP_CONFIG_CHANGE_CHANGED;
					/* there is a value already, free it */
					free(str);
					*(void **)dst = strdup(value);
				}
			} else {
				changed = SCCP_CONFIG_CHANGE_CHANGED;
				*(void **)dst = strdup(value);
			}

		} else if (!sccp_strlen_zero(str)) {
			changed = SCCP_CONFIG_CHANGE_CHANGED;
			/* there is a value already, free it */
			free(str);
			*(void **)dst = NULL;
		}
		break;

	case SCCP_CONFIG_DATATYPE_INT:
		if (!sccp_strlen_zero(value)) {
			intnum = atoi(value);
			if ((*(int *)dst) != intnum) {
				*(int *)dst = intnum;
				changed = SCCP_CONFIG_CHANGE_CHANGED;
			}
		}
		break;

	case SCCP_CONFIG_DATATYPE_BOOLEAN:
		bool = sccp_true(value);

		if (*(boolean_t *) dst != bool) {
			*(boolean_t *) dst = sccp_true(value);
			changed = SCCP_CONFIG_CHANGE_CHANGED;
		}
		break;

	case SCCP_CONFIG_DATATYPE_GENERIC:
		if (sccpConfigOption->converter_f) {
			changed = sccpConfigOption->converter_f(dst, sccpConfigOption->size, value, segment);
		}
		break;

	default:
		pbx_log(LOG_WARNING, "Unknown param at %s='%s'\n", name, value);
		return SCCP_CONFIG_NOUPDATENEEDED;
	}

	/* set changed value if changed */
	if (SCCP_CONFIG_CHANGE_CHANGED == changed) {
		sccp_log(DEBUGCAT_CONFIG) (VERBOSE_PREFIX_2 "config parameter %s='%s' in line %d changed. %s\n", name, value, lineno, SCCP_CONFIG_NEEDDEVICERESET==sccpConfigOption->change ? "(causes device reset)": "");
		changes = sccpConfigOption->change;
	}

	return changes;
}

/*!
 * \brief Set SCCP obj defaults from predecessor (device / global)
 *
 * check if we can find the param name in the segment specified and retrieving its value or default value
 * copy the string from the defaultSegment and run through the converter again
 */
void sccp_config_set_defaults(void *obj, const sccp_config_segment_t segment, const uint8_t alreadySetEntries[], uint8_t arraySize)
{
	uint8_t i = 0;
	//uint8_t arraySize = 0;
	sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_1 "setting %s defaults\n", sccp_find_segment(segment)->name);
	const SCCPConfigOption *sccpDstConfig = sccp_find_segment(segment)->config;
	const SCCPConfigOption *sccpDefaultConfigOption;
	sccp_device_t *my_device = NULL;
	sccp_line_t *my_line = NULL;
	sccp_device_t *ref_device = NULL;						/* need to find a way to find the default device to copy */
	int flags;									/* enum wrapper */
	int type;									/* enum wrapper */
	const char *value="";								/*! \todo retrieve value from correct segment */
	const char *variable_block_name="";
	
	/* check if not already set using it's own parameter in the sccp.conf file */
	switch (segment) {
		case SCCP_CONFIG_GLOBAL_SEGMENT:
			arraySize = ARRAY_LEN(sccpGlobalConfigOptions);
			variable_block_name=strdupa("general");
			sccp_log(DEBUGCAT_CORE) (VERBOSE_PREFIX_1 "setting [general] defaults\n");
			break;
		case SCCP_CONFIG_DEVICE_SEGMENT:
			my_device = &(*(sccp_device_t *)obj);
			variable_block_name=strdupa((const char *)my_device->id);
			arraySize = ARRAY_LEN(sccpDeviceConfigOptions);
			sccp_log(DEBUGCAT_CORE) (VERBOSE_PREFIX_1 "setting device[%s] defaults\n", my_device ? my_device->id : "NULL");
			break;
		case SCCP_CONFIG_LINE_SEGMENT:			
			my_line = &(*(sccp_line_t *)obj);			
			variable_block_name=strdupa((const char *)my_line->id);
			arraySize = ARRAY_LEN(sccpLineConfigOptions);
			sccp_log(DEBUGCAT_CORE) (VERBOSE_PREFIX_1 "setting line[%s] defaults\n", my_line ? my_line->name : "NULL");
			break;
		case SCCP_CONFIG_SOFTKEY_SEGMENT:
		        ast_log(LOG_ERROR, "softkey default loading not implemented yet\n");
			break;
	}

	/* find the defaultValue, first check the reference, if no reference is specified, us the local defaultValue */
	for (i = 0; i < arraySize; i++) {			
		flags = sccpDstConfig[i].flags;
		type = sccpDstConfig[i].type;

		// if (flags) { 
		if (alreadySetEntries[i]==0 && !(flags & SCCP_CONFIG_FLAG_OBSOLETE) ) {		// has not been set already
		        sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_2 "config parameter %s looking for default (flags: %d, type: %d)\n", sccpDstConfig[i].name, flags, type);
			switch (flags) {
			case SCCP_CONFIG_FLAG_GET_DEVICE_DEFAULT:
			        sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_2 "config parameter %s refering to device default\n", sccpDstConfig[i].name);
				ref_device = &(*(sccp_device_t *) obj);
				
				// get value from cfg->device category
				value = (char *)pbx_variable_retrieve(GLOB(cfg), (const char *)ref_device->id, (const char *)sccpDstConfig[i].name);
				if (!value) {
					// get defaultValue from default_segment
					sccpDefaultConfigOption = sccp_find_config(SCCP_CONFIG_DEVICE_SEGMENT, sccpDstConfig[i].name);
					if(sccpDefaultConfigOption) {
					        sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_2 "config parameter %s found value:%s in device source segment\n", sccpDstConfig[i].name, value);
						value = sccpDefaultConfigOption->defaultValue;
					} else {
					        sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_2 "config parameter %s not found in device source segment\n", sccpDstConfig[i].name);
					}
				} else {
				        sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_2 "config parameter %s found value:%s in sccp.conf\n", sccpDstConfig[i].name, value);
				}
			case SCCP_CONFIG_FLAG_GET_GLOBAL_DEFAULT:
			        sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_2 "config parameter %s refering to device default\n", sccpDstConfig[i].name);
				// get value from cfg->general category
				value = (char *)pbx_variable_retrieve(GLOB(cfg), "general", sccpDstConfig[i].name);
				if (!value) {
					// get defaultValue from default_segment
					sccpDefaultConfigOption = sccp_find_config(SCCP_CONFIG_GLOBAL_SEGMENT, sccpDstConfig[i].name);
					if(sccpDefaultConfigOption) {
					        sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_2 "config parameter %s found value:%s in global source segment\n", sccpDstConfig[i].name, value);
						value = sccpDefaultConfigOption->defaultValue;
					} else {
					        sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_2 "config parameter %s not found in global source segment\n", sccpDstConfig[i].name);
					}
				} else {
				        sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_2 "config parameter %s found value:%s in sccp.conf\n", sccpDstConfig[i].name, value);
				}
			default:
				if (!value) {
					// get defaultValue from self
				        sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_2 "config parameter %s using local source segment default: %s -> %s\n", sccpDstConfig[i].name, value, sccpDstConfig[i].defaultValue);
					value = sccpDstConfig[i].defaultValue;
				}
				break;
			}
			if (value) {
		        	if (!sccp_strlen_zero(value)) {
       				        sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_2 "config parameter %s using default %s\n", sccpDstConfig[i].name, value);
       					// get value from the config file and run through parser
        				sccp_config_object_setValue(obj, sccpDstConfig[i].name, value, 0, segment);
				}
				value=NULL; 
			}
		}
	}
}

/*!
 * \brief Config Converter/Parser for Debug
 */
sccp_value_changed_t sccp_config_parse_debug(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;

	uint32_t *debug_prev = (uint32_t *) dest;
	uint32_t debug_new = 0;

	char *debug_arr[1];

	debug_arr[0] = strdup(value);

	debug_new = sccp_parse_debugline(debug_arr, 0, 1, debug_new);
	if (debug_new != *debug_prev) {
		*(uint32_t *) dest = debug_new;
		changed = SCCP_CONFIG_CHANGE_CHANGED;
	}
	sccp_free(debug_arr[0]);
	return changed;
}

/*!
 * \brief Config Converter/Parser for Bind Address
 *
 */
sccp_value_changed_t sccp_config_parse_ipaddress(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;

	struct ast_hostent ahp;
	struct hostent *hp;

	struct sockaddr_in *bindaddr_prev = &(*(struct sockaddr_in *)dest);

	if (!(hp = pbx_gethostbyname(value, &ahp))) {
		pbx_log(LOG_WARNING, "Invalid address: %s. SCCP disabled\n", value);
		return SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}
	
	if (&bindaddr_prev->sin_addr != NULL) {
//  	        if (sccp_strcmp(pbx_inet_ntoa(bindaddr_prev->sin_addr), value)) {
  	        if (sccp_strcmp(pbx_inet_ntoa(bindaddr_prev->sin_addr), hp->h_addr)) {
			memcpy(&bindaddr_prev->sin_addr, hp->h_addr, sizeof(bindaddr_prev->sin_addr));
			changed = SCCP_CONFIG_CHANGE_CHANGED;
		}
	} else {
		memcpy(&bindaddr_prev->sin_addr, hp->h_addr, sizeof(bindaddr_prev->sin_addr));
		changed = SCCP_CONFIG_CHANGE_CHANGED;
	}
	return changed;
}

/*!
 * \brief Config Converter/Parser for Port
 */
sccp_value_changed_t sccp_config_parse_port(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;

	int new_port;
	struct sockaddr_in *bindaddr_prev = &(*(struct sockaddr_in *)dest);

	if (sscanf(value, "%i", &new_port) == 1) {
		if (&bindaddr_prev->sin_port != NULL) {				// reload
			if (bindaddr_prev->sin_port != htons(new_port)) {
				bindaddr_prev->sin_port = htons(new_port);
				changed = SCCP_CONFIG_CHANGE_CHANGED;
			}
		} else {
			bindaddr_prev->sin_port = htons(new_port);
			changed = SCCP_CONFIG_CHANGE_CHANGED;
		}
	} else {
		pbx_log(LOG_WARNING, "Invalid port number '%s'\n", value);
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}

	return changed;
}

/*!
 * \brief Config Converter/Parser for BlindTransferIndication
 */
sccp_value_changed_t sccp_config_parse_blindtransferindication(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	boolean_t blindtransferindication = *(boolean_t *) dest;

	if (!strcasecmp(value, "moh")) {
		blindtransferindication = SCCP_BLINDTRANSFER_MOH;
	} else if (!strcasecmp(value, "ring")) {
		blindtransferindication = SCCP_BLINDTRANSFER_RING;
	} else {
		pbx_log(LOG_WARNING, "Invalid blindtransferindication value, should be 'moh' or 'ring'\n");
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}

	if (*(boolean_t *) dest != blindtransferindication) {
		changed = SCCP_CONFIG_CHANGE_CHANGED;
		*(boolean_t *) dest = blindtransferindication;
	}
	return changed;
}

/*!
 * \brief Config Converter/Parser for ProtocolVersion
 *
 * \todo Can we completely loose the option to set the protocol version and just embed the max_protocol version in the source
 */
/*
sccp_value_changed_t sccp_config_parse_protocolversion(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	int protocolversion = *(int *)dest;
	int new_value;

	if (sscanf(value, "%i", &new_value) == 1) {
		if (new_value < SCCP_DRIVER_SUPPORTED_PROTOCOL_LOW) {
			new_value = SCCP_DRIVER_SUPPORTED_PROTOCOL_LOW;
		} else if (new_value > SCCP_DRIVER_SUPPORTED_PROTOCOL_HIGH) {
			new_value = SCCP_DRIVER_SUPPORTED_PROTOCOL_HIGH;
		}
		if (protocolversion != new_value) {
			changed = SCCP_CONFIG_CHANGE_CHANGED;
			*(int *)dest = new_value;
		}
	} else {
		pbx_log(LOG_WARNING, "Invalid protocol version value, has to be a number between '%d' and '%d'\n", SCCP_DRIVER_SUPPORTED_PROTOCOL_LOW, SCCP_DRIVER_SUPPORTED_PROTOCOL_HIGH);
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}
	return changed;
}
*/

/*!
 * \brief Config Converter/Parser for Call Answer Order
 */
sccp_value_changed_t sccp_config_parse_callanswerorder(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	call_answer_order_t callanswerorder = *(call_answer_order_t *) dest;
	call_answer_order_t new_value;

	if (!strcasecmp(value, "oldestfirst")) {
		new_value = ANSWER_OLDEST_FIRST;
	} else if (!strcasecmp(value, "lastfirst")) {
		new_value = ANSWER_LAST_FIRST;
	} else {
		return SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}

	if (callanswerorder != new_value) {
		changed = SCCP_CONFIG_CHANGE_CHANGED;
		*(call_answer_order_t *) dest = new_value;
	}
	return changed;
}

/*!
 * \brief Config Converter/Parser for RegContext
 */
sccp_value_changed_t sccp_config_parse_regcontext(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;

	if (strcasecmp(&(*(char *)dest), value)) {
		*(char **)dest = strdup(value);
		changed = SCCP_CONFIG_CHANGE_CHANGED;
	} else {
		return SCCP_CONFIG_CHANGE_NOCHANGE;
	}
	return changed;

}

/*!
 * \brief Config Converter/Parser for Codec Preferences
 *
 * \todo need check to see if preferred_codecs has changed
 * \todo do we need to reduce the preferences by the pbx -> skinny codec mapping ?
 */
sccp_value_changed_t sccp_config_parse_codec_preferences(void *dest, const size_t size, const char *value, const boolean_t allow, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	skinny_codec_t *preferred_codecs = &(*(skinny_codec_t *) dest);

//	skinny_codec_t preferred_codecs_prev[SKINNY_MAX_CAPABILITIES];
//	memcpy(preferred_codecs_prev, preferred_codecs, sizeof(preferred_codecs));

	if (!sccp_parse_allow_disallow(preferred_codecs, NULL, value, allow)) {
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	} else if (1==1){		/*\todo implement check */
		changed = SCCP_CONFIG_CHANGE_CHANGED;
	}
	return changed;
}

/*!
 * \brief Config Converter/Parser for Allow Codec Preferences
 */
sccp_value_changed_t sccp_config_parse_allow_codec(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	return sccp_config_parse_codec_preferences(dest, size, value, TRUE, segment);
}

/*!
 * \brief Config Converter/Parser for Disallow Codec Preferences
 */
sccp_value_changed_t sccp_config_parse_disallow_codec(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	return sccp_config_parse_codec_preferences(dest, size, value, FALSE, segment);
}

/*!
 * \brief Config Converter/Parser for Permit IP
 *
 * \todo need check to see if ha has changed
 */
sccp_value_changed_t sccp_config_parse_permit(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;

	struct sccp_ha **ha = dest;

	if (!strcasecmp(value,"internal")) {
		sccp_append_ha("permit", "10.0.0.0/255.0.0.0", *ha, NULL);
		sccp_append_ha("permit", "172.16.0.0/255.224.0.0", *ha, NULL);
		sccp_append_ha("permit", "192.168.1.0/255.255.255.0", *ha, NULL);
	} else {
		sccp_append_ha("permit", value, *ha, NULL);
	}

	return changed;
}

/*!
 * \brief Config Converter/Parser for Deny IP
 *
 * \todo need check to see if ha has changed
 */
sccp_value_changed_t sccp_config_parse_deny(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;

	struct sccp_ha **ha = dest;
	
	sccp_append_ha("deny", value, *ha, NULL);

	return changed;
}

/*!
 * \brief Config Converter/Parser for Buttons
 *
 * \todo Build a check to see if the button has changed 
 */
sccp_value_changed_t sccp_config_parse_button(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;

	char *buttonType = NULL, *buttonName = NULL, *buttonOption = NULL, *buttonArgs = NULL;
	char k_button[256];
	char *splitter;

#ifdef CS_DYNAMIC_CONFIG
	button_type_t type;
	unsigned i;
#endif
	sccp_log(0) (VERBOSE_PREFIX_3 "Found buttonconfig: %s\n", value);
	sccp_copy_string(k_button, value, sizeof(k_button));
	splitter = k_button;
	buttonType = strsep(&splitter, ",");
	buttonName = strsep(&splitter, ",");
	buttonOption = strsep(&splitter, ",");
	buttonArgs = splitter;
#ifdef CS_DYNAMIC_CONFIG
	for (i = 0; i < ARRAY_LEN(sccp_buttontypes) && strcasecmp(buttonType, sccp_buttontypes[i].text); ++i) ;
	if (i >= ARRAY_LEN(sccp_buttontypes)) {
		pbx_log(LOG_WARNING, "Unknown button type '%s'.\n", buttonType);
		return SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}
	type = sccp_buttontypes[i].buttontype;
#endif
//	sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_3 "ButtonType: %s\n", buttonType);
//	sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_3 "ButtonName: %s\n", buttonName);
//	sccp_log((DEBUGCAT_CONFIG + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_3 "ButtonOption: %s\n", buttonOption);

	changed = (sccp_value_changed_t)sccp_config_addButton(dest, 0, type, buttonName ? pbx_strip(buttonName) : buttonType, buttonOption ? pbx_strip(buttonOption) : NULL, buttonArgs ? pbx_strip(buttonArgs) : NULL);

	return changed;
}

/*!
 * \brief Config Converter/Parser for Permit Hosts
 *
 * \todo maybe add new DATATYPE_LIST to add string param value to LIST_HEAD to make a generic parser
 * \todo need check to see if  has changed
 */
sccp_value_changed_t sccp_config_parse_permithosts(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
//	sccp_hostname_t *permithost = &(*(sccp_hostname_t *) dest);
	sccp_hostname_t *permithost = NULL;

	if ((permithost = sccp_malloc(sizeof(sccp_hostname_t)))) {
		if (strcasecmp(permithost->name, value)) {
			sccp_copy_string(permithost->name, value, sizeof(permithost->name));
			SCCP_LIST_INSERT_HEAD(&(*(SCCP_LIST_HEAD(, sccp_hostname_t) *) dest), permithost, list);
			changed = SCCP_CONFIG_CHANGE_CHANGED;
		}
	} else {
		pbx_log(LOG_WARNING, "Error getting memory to assign hostname '%s' (malloc)\n", value);
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}
	return changed;
}

/*!
 * \todo make more generic
 * \todo cleanup original implementation in sccp_utils.c
 */
sccp_value_changed_t sccp_config_parse_addons(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	int addon_type;
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;

	SCCP_LIST_HEAD(, sccp_addon_t) * addonList = dest;

	// checking if addontype is known
	if (!strcasecmp(value, "7914"))
		addon_type = SKINNY_DEVICETYPE_CISCO7914;
	else if (!strcasecmp(value, "7915"))
		addon_type = SKINNY_DEVICETYPE_CISCO7915;
	else if (!strcasecmp(value, "7916"))
		addon_type = SKINNY_DEVICETYPE_CISCO7916;
	else {
		sccp_log(1) (VERBOSE_PREFIX_3 "SCCP: Unknown addon type (%s)\n", value);
		return SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}

	/*!\todo check allowed addons during the registration process, so we can use skinny device type instead of user defined type -MC */
	// checking if addontype is supported by devicetype
//      if (!((addon_type == SKINNY_DEVICETYPE_CISCO7914) &&
//            ((!strcasecmp(d->config_type, "7960")) ||
//             (!strcasecmp(d->config_type, "7961")) ||
//             (!strcasecmp(d->config_type, "7962")) ||
//             (!strcasecmp(d->config_type, "7965")) ||
//             (!strcasecmp(d->config_type, "7970")) || (!strcasecmp(d->config_type, "7971")) || (!strcasecmp(d->config_type, "7975")))) && !((addon_type == SKINNY_DEVICETYPE_CISCO7915) && ((!strcasecmp(d->config_type, "7962")) || (!strcasecmp(d->config_type, "7965")) || (!strcasecmp(d->config_type, "7975")))) && !((addon_type == SKINNY_DEVICETYPE_CISCO7916) && ((!strcasecmp(d->config_type, "7962")) || (!strcasecmp(d->config_type, "7965")) || (!strcasecmp(d->config_type, "7975"))))) {
//              sccp_log(1) (VERBOSE_PREFIX_3 "SCCP: Configured device (%s) does not support the specified addon type (%s)\n", d->config_type, value);
//              return changed;
//      }

	// add addon_type to list head
	sccp_addon_t *addon = sccp_malloc(sizeof(sccp_addon_t));

	if (!addon) {
		sccp_log(1) (VERBOSE_PREFIX_3 "SCCP: Unable to allocate memory for a device addon\n");
		return changed;
	}
	memset(addon, 0, sizeof(sccp_addon_t));

	addon->type = addon_type;

	SCCP_LIST_INSERT_HEAD(addonList, addon, list);

	return SCCP_CONFIG_CHANGE_CHANGED;
}

/*!
 * \brief
 *
 * \todo malloc/calloc of privacyFeature necessary ?
 */
sccp_value_changed_t sccp_config_parse_privacyFeature(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	sccp_featureConfiguration_t privacyFeature;				// = malloc(sizeof(sccp_featureConfiguration_t));

	if (!strcasecmp(value, "full")) {
		privacyFeature.status = ~0;
		privacyFeature.enabled = TRUE;
	} else if (sccp_true(value) || !sccp_true(value)) {
		privacyFeature.status = 0;
		privacyFeature.enabled = sccp_true(value);
	} else {
		pbx_log(LOG_WARNING, "Invalid privacy value, should be 'full', 'on' or 'off'\n");
		return SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}

	if (privacyFeature.status != (*(sccp_featureConfiguration_t *)dest).status || privacyFeature.enabled != (*(sccp_featureConfiguration_t *)dest).enabled) {
		*(sccp_featureConfiguration_t *)dest = privacyFeature;
		changed = SCCP_CONFIG_CHANGE_CHANGED;
	}
	return changed;
}

sccp_value_changed_t sccp_config_parse_earlyrtp(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	sccp_channelState_t earlyrtp = 0;

	if (!strcasecmp(value, "none")) {
		earlyrtp = 0;
	} else if (!strcasecmp(value, "offhook")) {
		earlyrtp = SCCP_CHANNELSTATE_OFFHOOK;
	} else if (!strcasecmp(value, "dial")) {
		earlyrtp = SCCP_CHANNELSTATE_DIALING;
	} else if (!strcasecmp(value, "ringout")) {
		earlyrtp = SCCP_CHANNELSTATE_RINGOUT;
	} else if (!strcasecmp(value, "progress")) {
		earlyrtp = SCCP_CHANNELSTATE_PROGRESS;
	} else {
		pbx_log(LOG_WARNING, "Invalid earlyrtp state value, should be 'none', 'offhook', 'dial', 'ringout', 'progress'\n");
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}

	if (*(sccp_channelState_t *)dest != earlyrtp ) {
		*(sccp_channelState_t *)dest = earlyrtp;
		changed = SCCP_CONFIG_CHANGE_CHANGED;
	}
	return changed;
}

sccp_value_changed_t sccp_config_parse_dtmfmode(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	boolean_t dtmfmode = 0;

	if (!strcasecmp(value, "outofband")) {
		dtmfmode = SCCP_DTMFMODE_OUTOFBAND;
	} else if (!strcasecmp(value, "inband")) {
		dtmfmode = SCCP_DTMFMODE_INBAND;
	} else {
		pbx_log(LOG_WARNING, "Invalid dtmfmode value, should be either 'inband' or 'outofband'\n");
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}

	if (*(boolean_t *) dest != dtmfmode) {
		*(boolean_t *) dest = dtmfmode;
		changed = SCCP_CONFIG_CHANGE_CHANGED;
	}
	return changed;
}

sccp_value_changed_t sccp_config_parse_mwilamp(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	sccp_lampMode_t mwilamp = 0;

	if (!strcasecmp(value, "off")) {
		mwilamp = SKINNY_LAMP_OFF;
	} else if (!strcasecmp(value, "on")) {
		mwilamp = SKINNY_LAMP_ON;
	} else if (!strcasecmp(value, "wink")) {
		mwilamp = SKINNY_LAMP_WINK;
	} else if (!strcasecmp(value, "flash")) {
		mwilamp = SKINNY_LAMP_FLASH;
	} else if (!strcasecmp(value, "blink")) {
		mwilamp = SKINNY_LAMP_BLINK;
	} else {
		pbx_log(LOG_WARNING, "Invalid mwilamp value, should be one of 'off', 'on', 'wink', 'flash' or 'blink'\n");
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}

	if (*(sccp_lampMode_t *) dest != mwilamp) {
		*(sccp_lampMode_t *) dest = mwilamp;
		changed = SCCP_CONFIG_CHANGE_CHANGED;
	}
	return changed;
}

/*!
 * \brief Config Converter/Parser for Mailbox Value
 * \todo make checks for changes to make it more generic 
 * 
 */
sccp_value_changed_t sccp_config_parse_mailbox(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_mailbox_t *mailbox = NULL;
	char *context, *mbox = NULL;

	SCCP_LIST_HEAD(, sccp_mailbox_t) * mailboxList = dest;

	mbox = context = sccp_strdupa(value);
	boolean_t mailbox_exists = FALSE;

	strsep(&context, "@");

	// Check mailboxes list
	SCCP_LIST_TRAVERSE(mailboxList, mailbox, list) {
		if (!sccp_strcmp(mailbox->mailbox, mbox)) {
			mailbox_exists = TRUE;
		}
	}
	if ((!mailbox_exists) && (!sccp_strlen_zero(mbox))) {
		// Add mailbox entry
		mailbox = sccp_calloc(1, sizeof(*mailbox));
		if (NULL != mailbox) {
			mailbox->mailbox = sccp_strdup(mbox);
			mailbox->context = sccp_strdup(context);

			SCCP_LIST_INSERT_TAIL(mailboxList, mailbox, list);
		}
	}
	/* \todo when mbox is free-ed here, we get a segfault */
/*	if (mbox)
		sccp_free(mbox);
*/		
	return SCCP_CONFIG_CHANGE_CHANGED;
}

/*!
 * \brief Config Converter/Parser for Tos Value
 */
sccp_value_changed_t sccp_config_parse_tos(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	unsigned int tos;

	if (!pbx_str2tos(value, &tos)) {
		/* value is tos */
	} else if (sscanf(value, "%i", &tos) == 1) {
		tos = tos & 0xff;
	} else if (!strcasecmp(value, "lowdelay")) {
	 	tos = IPTOS_LOWDELAY;
	} else if (!strcasecmp(value, "throughput")) {
		tos = IPTOS_THROUGHPUT;
	} else if (!strcasecmp(value, "reliability")) {
		tos = IPTOS_RELIABILITY;

#if !defined(__NetBSD__) && !defined(__OpenBSD__) && !defined(SOLARIS)
	} else if (!strcasecmp(value, "mincost")) {
		tos = IPTOS_MINCOST;
#endif
	} else if (!strcasecmp(value, "none")) {
		tos = 0;
	} else {
#if !defined(__NetBSD__) && !defined(__OpenBSD__) && !defined(SOLARIS)
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
#else
		\changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
#endif
		tos = 0x68 & 0xff;
	}

	if ((*(unsigned int *)dest) != tos) {
		*(unsigned int *)dest = tos;
		changed = SCCP_CONFIG_CHANGE_CHANGED;
	}
	return changed;
}

/*!
 * \brief Config Converter/Parser for Cos Value
 */
sccp_value_changed_t sccp_config_parse_cos(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	unsigned int cos;

	if (sscanf(value, "%d", &cos) == 1) {
		if (cos > 7) {
			pbx_log(LOG_WARNING, "Invalid cos %d value, refer to QoS documentation\n", cos);
			return SCCP_CONFIG_CHANGE_INVALIDVALUE;
		}
	}

	if ((*(unsigned int *)dest) != cos) {
		*(unsigned int *)dest = cos;
		changed = SCCP_CONFIG_CHANGE_CHANGED;
	}

	return changed;
}

/*!
 * \brief Config Converter/Parser for AmaFlags Value
 */
sccp_value_changed_t sccp_config_parse_amaflags(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	int amaflags;

	amaflags = pbx_cdr_amaflags2int(value);

	if (amaflags < 0) {
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	} else {
		if ((*(int *)dest) != amaflags) {
			changed = SCCP_CONFIG_CHANGE_CHANGED;
			*(int *)dest = amaflags;
		}
	}
	return changed;
}

/*!
 * \brief Config Converter/Parser for Small Int Value (1<value<255)
 */
sccp_value_changed_t sccp_config_parse_smallint(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	int new_value;

	if (sscanf(value, "%i", &new_value) == 1) {
		if (new_value >= 0 && new_value <= 255) {
			if ((*(int *)dest) != new_value) {
				changed = SCCP_CONFIG_CHANGE_CHANGED;
				*(int *)dest = new_value;
			}
		} else {
			changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
		}
	} else {
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}

	return changed;
}

/*!
 * \brief Config Converter/Parser for Secundairy Dialtone Digits
 */
sccp_value_changed_t sccp_config_parse_secondaryDialtoneDigits(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	char *str = (char *)dest;
	
	if (strlen(value) <= 9 ) {
		if (strcasecmp(str, value)) {
			sccp_copy_string(str, value, 9);
			changed = SCCP_CONFIG_CHANGE_CHANGED;
		}
	} else {
		changed = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}
	
	return changed;
}

/*!
 * \brief Config Converter/Parser for Setvar Value
 */
sccp_value_changed_t sccp_config_parse_variables(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_CHANGED;

	PBX_VARIABLE_TYPE *newvar = NULL;
	PBX_VARIABLE_TYPE *prevVar = *(PBX_VARIABLE_TYPE **)dest;

	newvar = sccp_create_variable(value);
	if (newvar) {
		newvar->next = prevVar;
		*(PBX_VARIABLE_TYPE **)dest = newvar;
	} else {
		changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	}

	return changed;
}

/*!
 * \brief Config Converter/Parser for Callgroup/Pickupgroup Values
 *
 * \todo check changes to make the function more generic
 */
sccp_value_changed_t sccp_config_parse_group(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;

	char *piece;
	char *c;
	int start=0, finish=0, x;
	sccp_group_t group = 0;

	if (!sccp_strlen_zero(value)){
		c = ast_strdupa(value);
		
		while ((piece = strsep(&c, ","))) {
			if (sscanf(piece, "%30d-%30d", &start, &finish) == 2) {
				/* Range */
			} else if (sscanf(piece, "%30d", &start)) {
				/* Just one */
				finish = start;
			} else {
				ast_log(LOG_ERROR, "Syntax error parsing group configuration '%s' at '%s'. Ignoring.\n", value, piece);
				continue;
			}
			for (x = start; x <= finish; x++) {
				if ((x > 63) || (x < 0)) {
					ast_log(LOG_WARNING, "Ignoring invalid group %d (maximum group is 63)\n", x);
				} else{
					group |= ((ast_group_t) 1 << x);
				}
			}
		}
	}
	
	if( (*(sccp_group_t *) dest) != group){
		changed = SCCP_CONFIG_CHANGE_CHANGED;
		
		*(sccp_group_t *) dest = group;
	}
	return changed;
}

/*!
 * \brief Config Converter/Parser for Context
 */
sccp_value_changed_t sccp_config_parse_context(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	char *str = (char *)dest;
	if (strcasecmp(str, value) ) {
		changed = SCCP_CONFIG_CHANGE_CHANGED;
		pbx_copy_string(dest, value, size);
                if (!pbx_context_find((const char *)dest)) {
                        ast_log(LOG_WARNING,"The context '%s' you specified might not be available in the dialplan. Please check the sccp.conf\n", (char *)dest);
                }
	}else{
	        changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	}
	return changed;
}

/*!
 * \brief Config Converter/Parser for DND Values
 * \note 0/off is allow and 1/on is reject
 */
sccp_value_changed_t sccp_config_parse_dnd(void *dest, const size_t size, const char *value, const sccp_config_segment_t segment)
{
	sccp_value_changed_t changed = SCCP_CONFIG_CHANGE_NOCHANGE;
	int dndmode;

	if (!strcasecmp(value, "reject")) {
		dndmode = SCCP_DNDMODE_REJECT;
	} else if (!strcasecmp(value, "silent")) {
		dndmode = SCCP_DNDMODE_SILENT;
	} else if (!strcasecmp(value, "user")) {
		dndmode = SCCP_DNDMODE_USERDEFINED;
	} else if (!strcasecmp(value, "")){
		dndmode = SCCP_DNDMODE_OFF;
	} else {
		dndmode = sccp_true(value);
	}

	if ((*(int *)dest) != dndmode) {
		(*(int *)dest) = dndmode;
		changed = SCCP_CONFIG_CHANGE_CHANGED;
	}

	return changed;
}

/* end dyn config */

/*!
 * \brief add a Button to a device
 * \param buttonconfig_head pointer to the device->buttonconfig list
 * \param type          type of button
 * \param name          name
 * \param options       options
 * \param args          args
 *
 * \callgraph
 * \callergraph
 * 
 * \lock
 * 	- device->buttonconfig
 * 	   - globals
 *
 * \todo Build a check to see if the button has changed
 */
sccp_configurationchange_t sccp_config_addButton(void *buttonconfig_head, int index, button_type_t type, const char *name, const char *options, const char *args)
{
	sccp_buttonconfig_t *config = NULL;
//	boolean_t is_new = FALSE;
	int highest_index = 0;
	sccp_configurationchange_t changes = SCCP_CONFIG_NOUPDATENEEDED;

	SCCP_LIST_HEAD(, sccp_buttonconfig_t) * buttonconfigList = buttonconfig_head;

	sccp_log(DEBUGCAT_CONFIG) (VERBOSE_PREFIX_1 "SCCP: Loading/Checking Button Config\n");
	SCCP_LIST_LOCK(buttonconfigList);
	SCCP_LIST_TRAVERSE(buttonconfigList, config, list) {
		// check if the button is to be deleted to see if we need to replace it
		if (index==0 && config->pendingDelete && config->type == type) {
			if (config->type == EMPTY || !strcmp(config->label, name)) {
				sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "Found Existing button at %d (Being Replaced)\n", config->index);
				index = config->index;
				break;
			}
		}
		highest_index = config->index;
	}

	if (index < 0) {
		index = highest_index + 1;
		config = NULL;
	}

	/* If a config at this position is not found, recreate one. */
	if (!config || config->index != index) {
		config = sccp_calloc(1, sizeof(*config));
		if (!config) {
			pbx_log(LOG_WARNING, "SCCP: sccp_config_addButton, memory allocation failed (calloc) failed\n");
			return SCCP_CONFIG_CHANGE_INVALIDVALUE;
		}

		config->index = index;
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "New %s Button %s at : %d:%d\n", sccp_buttontype2str(type), name, index, config->index);
		SCCP_LIST_INSERT_TAIL(buttonconfigList, config, list);
//		is_new = TRUE;
	} else {
		config->pendingDelete = 0;
		config->pendingUpdate = 1;
		changes = SCCP_CONFIG_CHANGE_CHANGED;
	}
	SCCP_LIST_UNLOCK(buttonconfigList);

	if (sccp_strlen_zero(name) || (type != LINE && !options) ) {
		sccp_log(0) (VERBOSE_PREFIX_1 "SCCP: Faulty Button Configuration found at index: %d", config->index);
		type = EMPTY;
		changes = SCCP_CONFIG_CHANGE_INVALIDVALUE;
	}

	switch (type) {
	case LINE:
	{
		struct composedId composedLineRegistrationId;
		memset(&composedLineRegistrationId, 0, sizeof(struct composedId));
		composedLineRegistrationId = sccp_parseComposedId(name, 80);

		if (	LINE==config->type && 
			!sccp_strcmp(config->label, name) &&
			!sccp_strcmp(config->button.line.name, composedLineRegistrationId.mainId) &&
			!sccp_strcasecmp(config->button.line.subscriptionId.number, composedLineRegistrationId.subscriptionId.number) &&	
			!sccp_strcmp(config->button.line.subscriptionId.name, composedLineRegistrationId.subscriptionId.name) &&	
			!sccp_strcmp(config->button.line.subscriptionId.aux, composedLineRegistrationId.subscriptionId.aux)
			) {
			if (!options || !sccp_strcmp(config->button.line.options, options)) {
				changes = SCCP_CONFIG_CHANGE_NOCHANGE;
				break;
			} else {
				changes = SCCP_CONFIG_CHANGE_NOCHANGE;
				break;
			}
		}
		config->type = LINE;

		sccp_copy_string(config->label, name, sizeof(config->label));
		sccp_copy_string(config->button.line.name, composedLineRegistrationId.mainId, sizeof(config->button.line.name));
		sccp_copy_string(config->button.line.subscriptionId.number, composedLineRegistrationId.subscriptionId.number, sizeof(config->button.line.subscriptionId.number));
		sccp_copy_string(config->button.line.subscriptionId.name, composedLineRegistrationId.subscriptionId.name, sizeof(config->button.line.subscriptionId.name));
		sccp_copy_string(config->button.line.subscriptionId.aux, composedLineRegistrationId.subscriptionId.aux, sizeof(config->button.line.subscriptionId.aux));
		if (options) {	
			sccp_copy_string(config->button.line.options, options, sizeof(config->button.line.options));
		}
		break;
	}
	case SPEEDDIAL:
		/* \todo check if values change */
		if (	SPEEDDIAL==config->type && 
			!sccp_strcmp(config->label, name) &&
			!sccp_strcmp(config->button.speeddial.ext, options)
			) {
			if (!args || !sccp_strcmp(config->button.speeddial.hint, args)) {
				changes = SCCP_CONFIG_CHANGE_NOCHANGE;
				break;
			}
		}
		config->type = SPEEDDIAL;

		sccp_copy_string(config->label, name, sizeof(config->label));
		sccp_copy_string(config->button.speeddial.ext, options, sizeof(config->button.speeddial.ext));
		if (args) {
			sccp_copy_string(config->button.speeddial.hint, args, sizeof(config->button.speeddial.hint));
		}
		break;
	case SERVICE:
		if (	SERVICE==config->type && 
			!sccp_strcmp(config->label, name) &&
			!sccp_strcmp(config->button.service.url, options)
			) {
			changes = SCCP_CONFIG_CHANGE_NOCHANGE;
			break;
		}
		/* \todo check if values change */
		config->type = SERVICE;

		sccp_copy_string(config->label, name, sizeof(config->label));
		sccp_copy_string(config->button.service.url, options, sizeof(config->button.service.url));
		break;
	case FEATURE:
		if (	FEATURE==config->type && 
			!sccp_strcmp(config->label, name) &&
			config->button.feature.id==sccp_featureStr2featureID(options)
			) {
			if (!args || !sccp_strcmp(config->button.feature.options, args)) {
				changes = SCCP_CONFIG_CHANGE_NOCHANGE;
				break;
			}
		}
		/* \todo check if values change */
		config->type = FEATURE;
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON | DEBUGCAT_BUTTONTEMPLATE)) (VERBOSE_PREFIX_3 "featureID: %s\n", options);

		sccp_copy_string(config->label, name, sizeof(config->label));
		config->button.feature.id = sccp_featureStr2featureID(options);

		if (args) {
			sccp_copy_string(config->button.feature.options, args, sizeof(config->button.feature.options));
			sccp_log(0) (VERBOSE_PREFIX_3 "Arguments present on feature button: %d\n", config->instance);
		}

		sccp_log((DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON | DEBUGCAT_BUTTONTEMPLATE)) (VERBOSE_PREFIX_3 "Configured feature button with featureID: %s args: %s\n", options, args);

		break;
	case EMPTY:
		if (	EMPTY==config->type ) {
			changes = SCCP_CONFIG_CHANGE_NOCHANGE;
			break;
		}
		/* \todo check if values change */
		config->type = EMPTY;
		break;
	}
	return changes;
}

/*!
 * \brief Build Line
 * \param v Asterisk Variable
 * \param lineName Name of line as char
 * \param isRealtime is Realtime as Boolean
 *
 * \callgraph
 * \callergraph
 * 
 * \lock
 * 	- line
 * 	  - see sccp_line_changed()
 */
sccp_line_t *sccp_config_buildLine(sccp_line_t *l, PBX_VARIABLE_TYPE *v, const char *lineName, boolean_t isRealtime)
{
	sccp_configurationchange_t res = sccp_config_applyLineConfiguration(l, v);
#ifdef CS_SCCP_REALTIME
	l->realtime = isRealtime;
#endif
#ifdef CS_DYNAMIC_CONFIG
	if (res == SCCP_CONFIG_NEEDDEVICERESET && l && l->pendingDelete) {
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CORE)) (VERBOSE_PREFIX_1 "SCCP: major changes for line '%s' detected, device reset required -> pendingUpdate=1\n", l->id);
		l->pendingUpdate = 1;
	}
	sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "%s: Removing pendingDelete\n", l->name);
	l->pendingDelete = 0;
#endif	/* CS_DYNAMIC_CONFIG */
	return l;
}

/*!
 * \brief Build Device
 * \param v Asterisk Variable
 * \param deviceName Name of device as char
 * \param isRealtime is Realtime as Boolean
 *
 * \callgraph
 * \callergraph
 * 
 * \lock
 * 	- device
 * 	  - see sccp_device_changed()
 */
sccp_device_t *sccp_config_buildDevice(sccp_device_t *d, PBX_VARIABLE_TYPE *v, const char *deviceName, boolean_t isRealtime)
{
	sccp_configurationchange_t res = sccp_config_applyDeviceConfiguration(d, v);
#ifdef CS_SCCP_REALTIME
	d->realtime = isRealtime;
#endif
#ifdef CS_DYNAMIC_CONFIG
	if (res == SCCP_CONFIG_NEEDDEVICERESET && d && d->pendingDelete) {
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CORE)) (VERBOSE_PREFIX_1 "%s: major changes for device detected, device reset required -> pendingUpdate=1\n", d->id);
		d->pendingUpdate = 1;
	}
	sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "%s: Removing pendingDelete\n", d->id);
	d->pendingDelete = 0;
#endif	/* CS_DYNAMIC_CONFIG */

	return d;
}

/*!
 * \brief Get Configured Line from Asterisk Variable
 * \param v Asterisk Variable
 * \return Configured SCCP Line
 * \note also used by realtime functionality to line device from Asterisk Variable
 *
 * \callgraph
 * \callergraph
 * 
 * \lock
 * 	- line->mailboxes
 */
sccp_configurationchange_t sccp_config_applyGlobalConfiguration(PBX_VARIABLE_TYPE *v)
{
	sccp_configurationchange_t res = SCCP_CONFIG_NOUPDATENEEDED;
	uint8_t alreadySetEntries[ARRAY_LEN(sccpGlobalConfigOptions)];
	uint8_t i=0;
	
	memset(alreadySetEntries, 0, sizeof(alreadySetEntries));

	for (; v; v = v->next) {
		res |= sccp_config_object_setValue(sccp_globals, v->name, v->value, v->lineno, SCCP_CONFIG_GLOBAL_SEGMENT);
//		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "Update Needed (%d)\n", res);
		// mark entries as already set
		for (i=0;i<ARRAY_LEN(sccpGlobalConfigOptions);i++) {
			if (!strcasecmp(sccpGlobalConfigOptions[i].name,v->name)) {
				alreadySetEntries[i]=1;
			}
		}
	}
	sccp_config_set_defaults(sccp_globals, SCCP_CONFIG_GLOBAL_SEGMENT, alreadySetEntries, ARRAY_LEN(sccpGlobalConfigOptions));	
	
	return res;
}

/*!
 * \brief Parse sccp.conf and Create General Configuration
 * \param readingtype SCCP Reading Type
 *
 * \callgraph
 * \callergraph
 */
boolean_t sccp_config_general(sccp_readingtype_t readingtype)
{
	PBX_VARIABLE_TYPE *v;

	/* Cleanup for reload */
        if(GLOB(ha)) {
		sccp_free_ha(GLOB(ha));
		GLOB(ha) = NULL;
        }
        if(GLOB(localaddr)){
		sccp_free_ha(GLOB(localaddr));
		GLOB(localaddr) = NULL;
        }

	if (!GLOB(cfg)) {
		pbx_log(LOG_WARNING, "Unable to load config file sccp.conf, SCCP disabled\n");
		return FALSE;
	}

	/* read the general section */
	v = ast_variable_browse(GLOB(cfg), "general");
	if (!v) {
		pbx_log(LOG_WARNING, "Missing [general] section, SCCP disabled\n");
		return FALSE;
	}

	sccp_configurationchange_t res = sccp_config_applyGlobalConfiguration(v);
//	sccp_config_set_defaults(sccp_globals, SCCP_CONFIG_GLOBAL_SEGMENT);
	
#ifdef CS_DYNAMIC_CONFIG
	if (res == SCCP_CONFIG_NEEDDEVICERESET) {
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_1 "SCCP: major changes detected in globals, reset required -> pendingUpdate=1\n");
		GLOB(pendingUpdate = 1);
	}
#endif

	/* setup bindaddress */
	if (!ntohs(GLOB(bindaddr.sin_port))) {
		GLOB(bindaddr.sin_port) = ntohs(DEFAULT_SCCP_PORT);
	}
	GLOB(bindaddr.sin_family) = AF_INET;

	/* setup hostname -> externip */
	/* \todo change using singular h_addr to h_addr_list (name may resolve to multiple ip-addresses */
	struct ast_hostent ahp;
	struct hostent *hp;

	if (!sccp_strlen_zero(GLOB(externhost))) {
		if (!(hp = pbx_gethostbyname(GLOB(externhost), &ahp))) {
			pbx_log(LOG_WARNING, "Invalid address resolution for externhost keyword: %s\n", GLOB(externhost));
		} else {
			memcpy(&GLOB(externip.sin_addr), hp->h_addr, sizeof(GLOB(externip.sin_addr)));
			time(&GLOB(externexpire));
		}
	}

	/* setup regcontext */
	char newcontexts[SCCP_MAX_CONTEXT];
	char oldcontexts[SCCP_MAX_CONTEXT];
	char *stringp, *context, *oldregcontext;

	sccp_copy_string(newcontexts, GLOB(regcontext), sizeof(newcontexts));
	stringp = newcontexts;

	sccp_copy_string(oldcontexts, GLOB(used_context), sizeof(oldcontexts));	// Initialize copy of current regcontext for later use in removing stale contexts
	oldregcontext = oldcontexts;

	cleanup_stale_contexts(stringp, oldregcontext);				// Let's remove any contexts that are no longer defined in regcontext

	while ((context = strsep(&stringp, "&"))) {				// Create contexts if they don't exist already
		sccp_copy_string(GLOB(used_context), context, sizeof(GLOB(used_context)));
		pbx_context_find_or_create(NULL, NULL, context, "SCCP");
	}

	return TRUE;
}

/*!
 * \brief Cleanup Stale Contexts (regcontext)
 * \param new New Context as Character
 * \param old Old Context as Character
 */
void cleanup_stale_contexts(char *new, char *old)
{
	char *oldcontext, *newcontext, *stalecontext, *stringp, newlist[SCCP_MAX_CONTEXT];

	while ((oldcontext = strsep(&old, "&"))) {
		stalecontext = '\0';
		sccp_copy_string(newlist, new, sizeof(newlist));
		stringp = newlist;
		while ((newcontext = strsep(&stringp, "&"))) {
			if (sccp_strcmp(newcontext, oldcontext) == 0) {
				/* This is not the context you're looking for */
				stalecontext = '\0';
				break;
			} else if (sccp_strcmp(newcontext, oldcontext)) {
				stalecontext = oldcontext;
			}

		}
		if (stalecontext)
			ast_context_destroy(ast_context_find(stalecontext), "SCCP");
	}
}

/*!
 * \brief Read Lines from the Config File
 *
 * \param readingtype as SCCP Reading Type
 * \since 10.01.2008 - branche V3
 * \author Marcello Ceschia
 *
 * \callgraph
 * \callergraph
 * 
 * \lock
 * 	- lines
 * 	  - see sccp_config_applyLineConfiguration()
 */
void sccp_config_readDevicesLines(sccp_readingtype_t readingtype)
{
//      struct ast_config *cfg = NULL;

	char *cat = NULL;
	PBX_VARIABLE_TYPE *v = NULL;
	uint8_t device_count = 0;
	uint8_t line_count = 0;
	
	sccp_line_t *l;
	sccp_device_t *d;
	boolean_t is_new=FALSE;

	sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_1 "Loading Devices and Lines from config\n");

#ifdef CS_DYNAMIC_CONFIG
	sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_1 "Checking Reading Type\n");
	if (readingtype == SCCP_CONFIG_READRELOAD) {
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "Device Pre Reload\n");
		sccp_device_pre_reload();
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "Line Pre Reload\n");
		sccp_line_pre_reload();
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "Softkey Pre Reload\n");
		sccp_softkey_pre_reload();
	}
#endif

	if (!GLOB(cfg)) {
		pbx_log(LOG_NOTICE, "Unable to load config file sccp.conf, SCCP disabled\n");
		return;
	}
	
	while ((cat = pbx_category_browse(GLOB(cfg), cat))) {

		const char *utype;

		if (!strcasecmp(cat, "general"))
			continue;

		utype = pbx_variable_retrieve(GLOB(cfg), cat, "type");

		if (!utype) {
			pbx_log(LOG_WARNING, "Section '%s' is missing a type parameter\n", cat);
			continue;
		} else if (!strcasecmp(utype, "device")) {
			// check minimum requirements for a device
			if (sccp_strlen_zero(pbx_variable_retrieve(GLOB(cfg), cat, "devicetype"))) {
				pbx_log(LOG_WARNING, "Unknown type '%s' for '%s' in %s\n", utype, cat, "sccp.conf");
				continue;
			} else {
				v = ast_variable_browse(GLOB(cfg), cat);
				
				// Try to find out if we have the device already on file.
				// However, do not look into realtime, since
				// we might have been asked to create a device for realtime addition,
				// thus causing an infinite loop / recursion.
				d = sccp_device_find_byid(cat, FALSE);

				/* create new device with default values */
				if (!d) {
					d = sccp_device_create();
					sccp_copy_string(d->id, cat, sizeof(d->id));		/* set device name */
					sccp_device_addToGlobals(d);
					is_new = TRUE;
					device_count++;
				}else{
#ifdef CS_DYNAMIC_CONFIG
					if(d->pendingDelete){
						d->pendingDelete = 0;
					}
#endif
				}
//				sccp_config_applyDeviceConfiguration(d, v);		/** load configuration and set defaults */
				sccp_config_buildDevice(d, v, cat, FALSE);
				sccp_log((DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_3 "found device %d: %s\n", device_count, cat);
				/* load saved settings from ast db */
				sccp_config_restoreDeviceFeatureStatus(d);
			}
		} else if (!strcasecmp(utype, "line")) {
			/* check minimum requirements for a line */
			if ((!(!sccp_strlen_zero(pbx_variable_retrieve(GLOB(cfg), cat, "label"))) && (!sccp_strlen_zero(pbx_variable_retrieve(GLOB(cfg), cat, "cid_name"))) && (!sccp_strlen_zero(pbx_variable_retrieve(GLOB(cfg), cat, "cid_num"))))) {
				pbx_log(LOG_WARNING, "Unknown type '%s' for '%s' in %s\n", utype, cat, "sccp.conf");
				continue;
			}
			line_count++;

			v = ast_variable_browse(GLOB(cfg), cat);
			
			/* check if we have this line already */
			l = sccp_line_find_byname_wo(cat, FALSE);
			if (!l) {
				l = sccp_line_create();
				is_new = TRUE;
				sccp_copy_string(l->name, cat, sizeof(l->name));
			} else {
				is_new = FALSE;
			}
						
			sccp_config_buildLine(l, v, cat, FALSE);
			if (is_new)
				sccp_line_addToGlobals(l);
			
			sccp_log((DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_3 "found line %d: %s\n", line_count, cat);
		} else if (!strcasecmp(utype, "softkeyset")) {
			sccp_log((DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_3 "read set %s\n", cat);
			v = ast_variable_browse(GLOB(cfg), cat);
			sccp_config_softKeySet(v, cat);
		}
	}

#ifdef CS_SCCP_REALTIME
	/* reload realtime lines */
	sccp_configurationchange_t res;

	SCCP_RWLIST_RDLOCK(&GLOB(lines));
	SCCP_RWLIST_TRAVERSE(&GLOB(lines), l, list) {
		if (l->realtime == TRUE && l != GLOB(hotline)->line) {
			sccp_log(DEBUGCAT_NEWCODE) (VERBOSE_PREFIX_3 "%s: reload realtime line\n", l->name);
			v = pbx_load_realtime(GLOB(realtimelinetable), "name", l->name, NULL);
#    ifdef CS_DYNAMIC_CONFIG
			/* we did not find this line, mark it for deletion */
			if (!v) {
				sccp_log(DEBUGCAT_NEWCODE) (VERBOSE_PREFIX_3 "%s: realtime line not found - set pendingDelete=1\n", l->name);
				l->pendingDelete = 1;
				continue;
			}
#    endif

			res = sccp_config_applyLineConfiguration(l, v);
			/* check if we did some changes that needs a device update */
#    ifdef CS_DYNAMIC_CONFIG
			if (res == SCCP_CONFIG_NEEDDEVICERESET) {
				l->pendingUpdate = 1;
			}
#    endif
			pbx_variables_destroy(v);
		}
	}
	SCCP_RWLIST_UNLOCK(&GLOB(lines));
	/* finished realtime line reload */
#endif

	if (GLOB(pendingUpdate)) {
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "Global param changed needing restart ->  Restart all device\n");
		sccp_device_t *device;
		SCCP_RWLIST_WRLOCK(&GLOB(devices));
		SCCP_RWLIST_TRAVERSE(&GLOB(devices), device, list) {
			if (!device->pendingDelete && !device->pendingUpdate) {
				device->pendingUpdate=1;
			}
		}	
		SCCP_RWLIST_UNLOCK(&GLOB(devices));
		GLOB(pendingUpdate)=0;
	}


#ifdef CS_DYNAMIC_CONFIG
	sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_1 "Checking Reading Type\n");
	if (readingtype == SCCP_CONFIG_READRELOAD) {
		/* IMPORTANT: The line_post_reload function may change the pendingUpdate field of
		 * devices, so it's really important to call it *before* calling device_post_real().
		 */
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "Line Post Reload\n");
		sccp_line_post_reload();
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "Device Post Reload\n");
		sccp_device_post_reload();
		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "Softkey Post Reload\n");
		sccp_softkey_post_reload();
	}
#endif
}

/*!
 * \brief Get Configured Line from Asterisk Variable
 * \param l SCCP Line
 * \param v Asterisk Variable
 * \return Configured SCCP Line
 * \note also used by realtime functionality to line device from Asterisk Variable
 *
 * \callgraph
 * \callergraph
 * 
 * \lock
 * 	- line->mailboxes
 */
sccp_configurationchange_t sccp_config_applyLineConfiguration(sccp_line_t * l, PBX_VARIABLE_TYPE *v)
{
	sccp_configurationchange_t res = SCCP_CONFIG_NOUPDATENEEDED;
	uint8_t alreadySetEntries[ARRAY_LEN(sccpLineConfigOptions)];
	uint8_t i=0;

	memset(alreadySetEntries, 0, sizeof(alreadySetEntries));
#ifdef CS_DYNAMIC_CONFIG
	if (l->pendingDelete) {
		/* removing variables */
		if (l->variables) {
			pbx_variables_destroy(l->variables);
			l->variables = NULL;
		}
	}
#endif
	for (; v; v = v->next) {
		res |= sccp_config_object_setValue(l, v->name, v->value, v->lineno, SCCP_CONFIG_LINE_SEGMENT);
		// mark entries as already set
//		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "%s: Update Needed (%d)\n", l->name, res);
		for (i=0;i<ARRAY_LEN(sccpLineConfigOptions);i++) {
			if (!strcasecmp(sccpLineConfigOptions[i].name,v->name)) {
				alreadySetEntries[i]=1;
			}
		}

	}
	sccp_config_set_defaults(l, SCCP_CONFIG_LINE_SEGMENT, alreadySetEntries, ARRAY_LEN(alreadySetEntries));
	
	return res;
}

/*!
 * \brief Apply Device Configuration from Asterisk Variable
 * \param d SCCP Device
 * \param v Asterisk Variable
 * \return Configured SCCP Device
 * \note also used by realtime functionality to line device from Asterisk Variable
 * \todo this function should be called sccp_config_applyDeviceConfiguration
 *
 * \callgraph
 * \callergraph
 * 
 * \lock
 * 	- device->addons
 * 	- device->permithosts
 */
sccp_configurationchange_t sccp_config_applyDeviceConfiguration(sccp_device_t * d, PBX_VARIABLE_TYPE * v)
{
	sccp_configurationchange_t res = SCCP_CONFIG_NOUPDATENEEDED;
	char *prev_var_name = NULL;
	uint8_t alreadySetEntries[ARRAY_LEN(sccpDeviceConfigOptions)];
	uint8_t i=0;

	memset(alreadySetEntries, 0, sizeof(alreadySetEntries));
#ifdef CS_DYNAMIC_CONFIG
	if (d->pendingDelete) {
		// remove all addons before adding them again (to find differences later on in sccp_device_change)
		sccp_addon_t *addon;

		SCCP_LIST_LOCK(&d->addons);
		while ((addon = SCCP_LIST_REMOVE_HEAD(&d->addons, list))) {
			sccp_free(addon);
			addon = NULL;
		}
		SCCP_LIST_UNLOCK(&d->addons);
		SCCP_LIST_HEAD_DESTROY(&d->addons);
		SCCP_LIST_HEAD_INIT(&d->addons);

		/* removing variables */
		if (d->variables) {
			pbx_variables_destroy(d->variables);
			d->variables = NULL;
		}

		/* removing permithosts */
		sccp_hostname_t *permithost;

		SCCP_LIST_LOCK(&d->permithosts);
		while ((permithost = SCCP_LIST_REMOVE_HEAD(&d->permithosts, list))) {
			sccp_free(permithost);
			permithost = NULL;
		}
		SCCP_LIST_UNLOCK(&d->permithosts);
		SCCP_LIST_HEAD_DESTROY(&d->permithosts);
		SCCP_LIST_HEAD_INIT(&d->permithosts);

		sccp_free_ha(d->ha);
		d->ha = NULL;
	}
#endif
	for (; v; v = v->next) {
		res |= sccp_config_object_setValue(d, v->name, v->value, v->lineno, SCCP_CONFIG_DEVICE_SEGMENT);
//		sccp_log((DEBUGCAT_NEWCODE | DEBUGCAT_CONFIG)) (VERBOSE_PREFIX_2 "%s: Update Needed (%d)\n", d->id, res);

		// mark entries as already set
		for (i=0;i<ARRAY_LEN(sccpDeviceConfigOptions);i++) {
			if (!strcasecmp(sccpDeviceConfigOptions[i].name,v->name)) {
				alreadySetEntries[i]=1;
			}
		}
	}
	if(prev_var_name)
		sccp_free(prev_var_name);

	sccp_config_set_defaults(d, SCCP_CONFIG_DEVICE_SEGMENT, alreadySetEntries, ARRAY_LEN(alreadySetEntries));

#ifdef CS_DEVSTATE_FEATURE
	sccp_device_lock(d);
	sccp_buttonconfig_t *config = NULL;
	sccp_devstate_specifier_t *dspec;
	
	SCCP_LIST_LOCK(&d->buttonconfig);
	SCCP_LIST_TRAVERSE(&d->buttonconfig, config, list) {
		if (config->type == FEATURE) {
			/* Check for the presence of a devicestate specifier and register in device list. */
			if ((SCCP_FEATURE_DEVSTATE == config->button.feature.id) && (strncmp("", config->button.feature.options, 254))) {
				dspec = sccp_calloc(1, sizeof(sccp_devstate_specifier_t));
				sccp_log(0) (VERBOSE_PREFIX_3 "Recognized devstate feature button: %d\n", config->instance);
				SCCP_LIST_LOCK(&d->devstateSpecifiers);
				sccp_copy_string(dspec->specifier, config->button.feature.options, sizeof(config->button.feature.options));
				SCCP_LIST_INSERT_TAIL(&d->devstateSpecifiers, dspec, list);
				SCCP_LIST_UNLOCK(&d->devstateSpecifiers);
			}
		}
	}
	SCCP_LIST_UNLOCK(&d->buttonconfig);
	sccp_device_unlock(d);
#endif

	return res;
}

/*!
 * \brief Find the Correct Config File
 * \return Asterisk Config Object as ast_config
 */
struct ast_config *sccp_config_getConfig()
{
	struct ast_flags config_flags = { CONFIG_FLAG_WITHCOMMENTS & CONFIG_FLAG_FILEUNCHANGED };

	if (sccp_strlen_zero(GLOB(config_file_name))) {
		GLOB(config_file_name) = "sccp.conf";
	}
	if (GLOB(cfg) != NULL) {
		pbx_config_destroy(GLOB(cfg));
	}

	GLOB(cfg) = pbx_config_load(GLOB(config_file_name), "chan_sccp", config_flags);

	if (CONFIG_STATUS_FILEMISSING == GLOB(cfg)) {
		pbx_log(LOG_WARNING, "Config file '%s' not found, aborting reload.\n", GLOB(config_file_name));
	} else if (CONFIG_STATUS_FILEUNCHANGED == GLOB(cfg)) {
		pbx_log(LOG_NOTICE, "Config file '%s' has not changed, aborting reload.\n", GLOB(config_file_name));
	} else if (CONFIG_STATUS_FILEINVALID == GLOB(cfg)) {
		pbx_log(LOG_WARNING, "Config file '%s' specified is not a valid config file, aborting reload.\n", GLOB(config_file_name));
	} else if (GLOB(cfg) && ast_variable_browse(GLOB(cfg), "devices")) {	/* Warn user when old entries exist in sccp.conf */
		pbx_log(LOG_WARNING, "\n\n --> You are using an old configuration format, please update '%s'!!\n --> Loading of module chan_sccp with current sccp.conf has terminated\n --> Check http://chan-sccp-b.sourceforge.net/doc_setup.shtml for more information.\n\n", GLOB(config_file_name));
		pbx_config_destroy(GLOB(cfg));
		return CONFIG_STATUS_FILEOLD;
	} else if (!ast_variable_browse(GLOB(cfg), "general")) {
		pbx_log(LOG_WARNING, "Missing [general] section, SCCP disabled\n");
		pbx_config_destroy(GLOB(cfg));
		return CONFIG_STATUS_FILE_NOT_SCCP;
	}
	return GLOB(cfg);
}

/*!
 * \brief Read a SoftKey configuration context
 * \param variable list of configuration variables
 * \param name name of this configuration (context)
 * 
 * \callgraph
 * \callergraph
 *
 * \lock
 *	- softKeySetConfig
 */
void sccp_config_softKeySet(PBX_VARIABLE_TYPE *variable, const char *name)
{
	int keySetSize;
	sccp_softKeySetConfiguration_t *softKeySetConfiguration = NULL;
	int keyMode = -1;
	unsigned int i = 0;

	sccp_log((DEBUGCAT_CONFIG | DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "start reading softkeyset: %s\n", name);

	
	SCCP_LIST_LOCK(&softKeySetConfig);
	SCCP_LIST_TRAVERSE(&softKeySetConfig, softKeySetConfiguration, list) {
		if(!strcasecmp(softKeySetConfiguration->name, name))
			break;
	}
	SCCP_LIST_UNLOCK(&softKeySetConfig);
	
	if(!softKeySetConfiguration){
		softKeySetConfiguration = sccp_calloc(1, sizeof(sccp_softKeySetConfiguration_t));
		memset(softKeySetConfiguration, 0, sizeof(softKeySetConfiguration));

		sccp_copy_string(softKeySetConfiguration->name, name, sizeof(softKeySetConfiguration->name));
		softKeySetConfiguration->numberOfSoftKeySets = 0;
	
		/* add new softkexSet to list */
		SCCP_LIST_LOCK(&softKeySetConfig);
		SCCP_LIST_INSERT_HEAD(&softKeySetConfig, softKeySetConfiguration, list);
		SCCP_LIST_UNLOCK(&softKeySetConfig);
	}

	while (variable) {
		keyMode = -1;
		sccp_log((DEBUGCAT_CONFIG | DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "softkeyset: %s \n", variable->name);
		if (!strcasecmp(variable->name, "type")) {

		} else if (!strcasecmp(variable->name, "onhook")) {
			keyMode = KEYMODE_ONHOOK;
		} else if (!strcasecmp(variable->name, "connected")) {
			keyMode = KEYMODE_CONNECTED;
		} else if (!strcasecmp(variable->name, "onhold")) {
			keyMode = KEYMODE_ONHOLD;
		} else if (!strcasecmp(variable->name, "ringin")) {
			keyMode = KEYMODE_RINGIN;
		} else if (!strcasecmp(variable->name, "offhook")) {
			keyMode = KEYMODE_OFFHOOK;
		} else if (!strcasecmp(variable->name, "conntrans")) {
			keyMode = KEYMODE_CONNTRANS;
		} else if (!strcasecmp(variable->name, "digitsfoll")) {
			keyMode = KEYMODE_DIGITSFOLL;
		} else if (!strcasecmp(variable->name, "connconf")) {
			keyMode = KEYMODE_CONNCONF;
		} else if (!strcasecmp(variable->name, "ringout")) {
			keyMode = KEYMODE_RINGOUT;
		} else if (!strcasecmp(variable->name, "offhookfeat")) {
			keyMode = KEYMODE_OFFHOOKFEAT;
		} else if (!strcasecmp(variable->name, "onhint")) {
			keyMode = KEYMODE_INUSEHINT;
		} else {
			// do nothing
		}

		if (keyMode == -1) {
			variable = variable->next;
			continue;
		}

		if (softKeySetConfiguration->numberOfSoftKeySets < (keyMode + 1)) {
			softKeySetConfiguration->numberOfSoftKeySets = keyMode + 1;
		}

		for (i = 0; i < (sizeof(SoftKeyModes) / sizeof(softkey_modes)); i++) {
			if (SoftKeyModes[i].id == keyMode) {
			  
				/* cleanup old value */
				if(softKeySetConfiguration->modes[i].ptr)
					sccp_free(softKeySetConfiguration->modes[i].ptr);
			  
				uint8_t *softkeyset = sccp_calloc(StationMaxSoftKeySetDefinition, sizeof(uint8_t));

				keySetSize = sccp_config_readSoftSet(softkeyset, variable->value);

				if (keySetSize > 0) {			  
					softKeySetConfiguration->modes[i].id = keyMode;
					softKeySetConfiguration->modes[i].ptr = softkeyset;
					softKeySetConfiguration->modes[i].count = keySetSize;
				} else {
					softKeySetConfiguration->modes[i].ptr = NULL;
					softKeySetConfiguration->modes[i].count = 0;
					sccp_free(softkeyset);
				}
			}
		}

		variable = variable->next;
	}

	
}

/*!
 * \brief Read a single SoftKeyMode (configuration values)
 * \param softkeyset SoftKeySet
 * \param data configuration values
 * \return number of parsed softkeys
 */
uint8_t sccp_config_readSoftSet(uint8_t * softkeyset, const char *data)
{
	int i = 0, j;

	char labels[256];
	char *splitter;
	char *label;

	if (!data)
		return 0;

	strcpy(labels, data);
	splitter = labels;
	while ((label = strsep(&splitter, ",")) != NULL && (i + 1) < StationMaxSoftKeySetDefinition) {
		softkeyset[i++] = sccp_config_getSoftkeyLbl(label);
	}

	for (j = i + 1; j < StationMaxSoftKeySetDefinition; j++) {
		softkeyset[j] = SKINNY_LBL_EMPTY;
	}
	return i;
}

/*!
 * \brief Get softkey label as int
 * \param key configuration value
 * \return SoftKey label as int of SKINNY_LBL_*. returns an empty button if nothing matched
 */
int sccp_config_getSoftkeyLbl(char *key)
{
	int i = 0;
	int size = sizeof(softKeyTemplate) / sizeof(softkeyConfigurationTemplate);

	for (i = 0; i < size; i++) {
		if (!strcasecmp(softKeyTemplate[i].configVar, key)) {
			return softKeyTemplate[i].softkey;
		}
	}
	sccp_log((DEBUGCAT_CONFIG | DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "softkeybutton: %s not defined", key);
	return SKINNY_LBL_EMPTY;
}

/*!
 * \brief Restore feature status from ast-db
 * \param device device to be restored
 * \todo restore cfwd feature
 *
 * \callgraph
 * \callergraph
 * 
 * \lock
 * 	- device->devstateSpecifiers
 */
void sccp_config_restoreDeviceFeatureStatus(sccp_device_t * device)
{
	if (!device)
		return;

#ifdef CS_DEVSTATE_FEATURE
	char buf[256] = "";
	sccp_devstate_specifier_t *specifier;
#endif

#ifndef ASTDB_FAMILY_KEY_LEN
#    define ASTDB_FAMILY_KEY_LEN 256
#endif

#ifndef ASTDB_RESULT_LEN
#    define ASTDB_RESULT_LEN 256
#endif

	char buffer[ASTDB_RESULT_LEN];
	char timebuffer[ASTDB_RESULT_LEN];
	int timeout;
	char family[ASTDB_FAMILY_KEY_LEN];

	sprintf(family, "SCCP/%s", device->id);

	/* dndFeature */
	if (PBX(feature_getFromDatabase)(family, "dnd", buffer, sizeof(buffer))) {
		if (!strcasecmp(buffer, "silent"))
			device->dndFeature.status = SCCP_DNDMODE_SILENT;
		else
			device->dndFeature.status = SCCP_DNDMODE_REJECT;
	} else {
		device->dndFeature.status = SCCP_DNDMODE_OFF;
	}

	/* monitorFeature */
	if (PBX(feature_getFromDatabase)(family, "monitor", buffer, sizeof(buffer))) {
		device->monitorFeature.status = 1;
	} else {
		device->monitorFeature.status = 0;
	}

	/* privacyFeature */
	if (PBX(feature_getFromDatabase)(family, "privacy", buffer, sizeof(buffer))) {
		sscanf(buffer, "%u", (unsigned int *)&device->privacyFeature.status);
	} else {
		device->privacyFeature.status = 0;
	}

	/* Message */
	if (PBX(feature_getFromDatabase) && PBX(feature_getFromDatabase) ("SCCP/message", "text", buffer, sizeof(buffer))) {
		if (!sccp_strlen_zero(buffer)) {
			if (PBX(feature_getFromDatabase) && PBX(feature_getFromDatabase) ("SCCP/message", "timeout", timebuffer, sizeof(timebuffer))) {
				sscanf(timebuffer, "%i", &timeout);
			}
			if (timeout) {
				sccp_dev_displayprinotify(device, buffer, 5, timeout);
			} else {	
				sccp_device_addMessageToStack(device, SCCP_MESSAGE_PRIORITY_IDLE, buffer);
			}
		}
	}

	/* lastDialedNumber */
	char lastNumber[SCCP_MAX_EXTENSION] = "";

	if (PBX(feature_getFromDatabase)(family, "lastDialedNumber", lastNumber, sizeof(lastNumber))) {
		sccp_copy_string(device->lastNumber, lastNumber, sizeof(device->lastNumber));
	}

	/* initialize so called priority feature */
	device->priFeature.status = 0x010101;
	device->priFeature.initialized = 0;

#ifdef CS_DEVSTATE_FEATURE
	/* Read and initialize custom devicestate entries */
	SCCP_LIST_LOCK(&device->devstateSpecifiers);
	SCCP_LIST_TRAVERSE(&device->devstateSpecifiers, specifier, list) {
		/* Check if there is already a devicestate entry */
		if (PBX(feature_getFromDatabase)(devstate_db_family, specifier->specifier, buf, sizeof(buf))) {
			sccp_log(DEBUGCAT_CONFIG) (VERBOSE_PREFIX_1 "%s: Found Existing Custom Devicestate Entry: %s, state: %s\n", device->id, specifier->specifier, buf);
		} else {
			/* If not present, add a new devicestate entry. Default: NOT_INUSE */
			PBX(feature_addToDatabase)(devstate_db_family, specifier->specifier, "NOT_INUSE");
			sccp_log(DEBUGCAT_CONFIG) (VERBOSE_PREFIX_1 "%s: Initialized Devicestate Entry: %s\n", device->id, specifier->specifier);
		}
		/* Register as generic hint watcher */
		/*! \todo Add some filtering in order to reduce number of unnecessarily triggered events.
		   Have to work out whether filtering with AST_EVENT_IE_DEVICE matches extension or hint device name. */
		snprintf(buf, 254, "Custom:%s", specifier->specifier);
		/* When registering for devstate events we wish to know if a single asterisk box has contributed
		   a change even in a rig of multiple asterisk with distributed devstate. This is to enable toggling
		   even then when otherwise the aggregate devicestate would obscure the change.
		   However, we need to force distributed devstate even on single asterisk boxes so to get the desired events. (-DD) */
#ifdef CS_NEW_DEVICESTATE
		ast_enable_distributed_devstate();
		specifier->sub = pbx_event_subscribe(AST_EVENT_DEVICE_STATE, sccp_devstateFeatureState_cb, "devstate subscription", device, AST_EVENT_IE_DEVICE, AST_EVENT_IE_PLTYPE_STR, strdup(buf), AST_EVENT_IE_END);
#endif		
	}
	SCCP_LIST_UNLOCK(&device->devstateSpecifiers);
#endif


}
