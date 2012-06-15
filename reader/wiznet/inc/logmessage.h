/*
 * logmessage.h
 *
 *  Created on: 04 Feb 2010
 *      Author: Wouter
 */

#ifndef LOGMESSAGE_H_
#define LOGMESSAGE_H_

#include "avrlibtypes.h"

#define LOG_ENTRY_HEADER  0xEE

typedef enum {
  lmSystemInitialized = 0,
  lmBiometricReaderEnabled,
  lmBiometricReaderDisabled,
  lmFingerprintsAuthenticated,
  lmFingerprintAuthorisationPeriodExpired,
  lmOTCAuthorisationPeriodExpired,
  lmOpenVaultDoorPeriodExpired,
  lmOpenMultivaultSafePeriodExpired,
  lmPressMultivaultButtonPeriodExpired,
  lmCloseVaultDoorPeriodExpired,
  lmSafeSelected,
  lmButtonEnabled,
  lmButtonPressed,
  lmVaultDoorButtonEnabled,
  lmVaultDoorButtonDisabled,
  lmSafeDoorUnlocked,
  lmSafeDoorOpened,
  lmSafeDoorClosed,
  lmLockoutPeriodCommenced,
  lmLockoutPeriodEnded,
  lmSideDoorOpened,
  lmSideDoorClosed,
  lmVaultDoorOpened,
  lmVaultDoorClosed,
  lmOuterDoorOpened,
  lmOuterDoorClosed,
  lmTamperDetected,
  lmCodeEntered,
  lmCodeAccepted,
  lmCodeRejectedInvalid,
  lmCodeRejectedUsed,
  lmOpeningRequest,
  lmMode1Request,
  lmMode2Request,
  lmMode3Request,
  lmSetToOperationalMode,
  lmSetToLoadingMode,
  lmSetToServiceMode,
  lmLoadingModeExpired,
  lmAdjustTimeFunctionSelected,
  lmAdjustVehicleNumberFunctionSelected,
  lmVehicleNumberChanged,
  lmTimeChanged,
  lmSetToBioOverideMode,
  lmBioOvrReset,
  lmSetBioOvrFunctionSelected
} eLogMessage;

typedef struct {
  u08 header;
  eLogMessage msg;
  u32 time;
} sLogMsgEvent;

typedef struct {
  sLogMsgEvent event;
  u08 safeNumber;
} sLogSafeSelected;

typedef struct {
  sLogMsgEvent event;
  u32 code;
} sLogCodeEntered;

typedef struct {
  sLogMsgEvent event;
  u16 vehNumber;
} sLogVehicleNumberChanged;

typedef struct {
  sLogMsgEvent event;
  s08 minutes;
  s08 hours;
} sLogTimeChanged;

#endif /* LOGMESSAGE_H_ */
