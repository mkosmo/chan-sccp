/*!
 * \file        sccp_feature.h
 * \brief       SCCP Feature Header
 * \author      Federico Santulli <fsantulli [at] users.sourceforge.net >
 * \author      Diederik de Groot <ddegroot [at] users.sourceforge.net >
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License. 
 *              See the LICENSE file at the top of the source tree.
 * \since       2009-01-16
 */
#pragma once
__BEGIN_C_EXTERN__

/*!
 * \brief SCCP Feature Configuration Structure
 */
struct sccp_feature_configuration {
	uint32_t previousStatus;										//!< Feature Previous State
	uint32_t status;											//!< Feature State
	boolean_t enabled;											//!< Feature Enabled
	boolean_t initialized;											//!< Feature Enabled
};

// callforward
SCCP_API void SCCP_CALL sccp_feat_handle_callforward(constLinePtr l, constDevicePtr d, sccp_cfwd_t type, channelPtr maybe_c, uint32_t lineInstance);

#ifdef CS_SCCP_PICKUP
SCCP_API void SCCP_CALL sccp_feat_handle_directed_pickup(constDevicePtr d, constLinePtr l, channelPtr maybe_c);
SCCP_API int SCCP_CALL sccp_feat_directed_pickup(constDevicePtr d, channelPtr c, uint32_t lineInstance, const char *exten);
SCCP_API int SCCP_CALL sccp_feat_grouppickup(constDevicePtr d, constLinePtr l, uint32_t lineInstance, channelPtr maybe_c);
#endif
SCCP_API void SCCP_CALL sccp_feat_voicemail(constDevicePtr d, uint8_t lineInstance);
SCCP_API void SCCP_CALL sccp_feat_idivert(constDevicePtr d, constLinePtr l, constChannelPtr c);
SCCP_API void SCCP_CALL sccp_feat_handle_conference(constDevicePtr d, constLinePtr l, uint8_t lineInstance, channelPtr channel);
SCCP_API void SCCP_CALL sccp_feat_conference_start(constDevicePtr device, const uint32_t lineInstance, channelPtr c);
SCCP_API void SCCP_CALL sccp_feat_join(constDevicePtr device, constLinePtr l, uint8_t lineInstance, channelPtr c);
SCCP_API void SCCP_CALL sccp_feat_conflist(devicePtr d, uint8_t lineInstance, constChannelPtr c);
SCCP_API void SCCP_CALL sccp_feat_handle_meetme(constLinePtr l, uint8_t lineInstance, constDevicePtr d);
SCCP_API void SCCP_CALL sccp_feat_meetme_start(channelPtr c);
SCCP_API void SCCP_CALL sccp_feat_handle_barge(constLinePtr l, uint8_t lineInstance, constDevicePtr d, channelPtr maybe_c);
SCCP_API int SCCP_CALL sccp_feat_singleline_barge(channelPtr c, const char *const exten);
SCCP_API void SCCP_CALL sccp_feat_handle_cbarge(constLinePtr l, uint8_t lineInstance, constDevicePtr d);
SCCP_API int SCCP_CALL sccp_feat_cbarge(constChannelPtr c, const char *const conferencenum);
SCCP_API void SCCP_CALL sccp_feat_adhocDial(constDevicePtr d, constLinePtr line);
SCCP_API void SCCP_CALL sccp_feat_changed(constDevicePtr device, constLineDevicePtr maybe_ld, sccp_feature_type_t featureType);
SCCP_API void SCCP_CALL sccp_feat_monitor(constDevicePtr device, constLinePtr no_line, uint32_t no_lineInstance, constChannelPtr maybe_channel);
__END_C_EXTERN__
// kate: indent-width 8; replace-tabs off; indent-mode cstyle; auto-insert-doxygen on; line-numbers on; tab-indents on; keep-extra-spaces off; auto-brackets off;
