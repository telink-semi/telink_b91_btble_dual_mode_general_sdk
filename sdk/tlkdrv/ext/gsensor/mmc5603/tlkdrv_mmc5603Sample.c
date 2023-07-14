/********************************************************************************************************
 * @file	tlkdrv_mmc5603Sample.c
 *
 * @brief	This is the source file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
/* This sample code teaches developer how to operate the MEMSIC alogrithm library
 * to get Yaw, Pitch and Roll respectively. if developer want to use it as a part
 * of his codes, he need to declare and define 3 functions by himself:
 * ReadPara;
 * Savepara;
 * please NOTE it is ONLY a sample code and can be changed freely by developers.
 */

#include "tlkapi/tlkapi_stdio.h"
#include "../tlkdrv_gsensor.h"
#if TLKDRV_GSENSOR_MMC5603_ENABLE
#include "tlkdrv_mmc5603Ctrl.h"
#include "tlkdrv_mmc5603MemsicAlg.h"
#include "tlkdrv_mmc5603MemsicCompass.h"
#include "math.h"
//#include "Customer.h"

#define TLKDRV_GEO_MMC5603_DBG_FLAG     ((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_GEO_MMC5603_DBG_SIGN     "[GEO]"

#define A_LAYOUT	4
#define M_LAYOUT	0

/* magnetic sensor soft iron calibration matrix, default is unit matrix */
float mag_smm[9] = { 1.0f, 0.0f, 0.0f, \
					0.0f, 1.0f, 0.0f, \
					0.0f, 0.0f, 1.0f};

/* default hard iron offset and magnetic field strength*/
float mag_hmm[4] = {0.0f,0.0f,0.0f,0.5f};

/* These variables are used to save the magnetic sensor calibrated data. */
float calMagX, calMagY, calMagZ;

/* These variables are used to save the calibrated orientation output. */
float fAzimuth, fPitch, fRoll;

/* This variable is used to save the calibration accuracy. */
int iAccuracy;

/* Read saved calibration parameters. These para is saved in somewhere that can not disappear when power-off
 * ReadPara function should be create by customer.If customer can not implement this function, should comment it out
 * If there is no parameter saved, ReadPara() should return -1, else return 1;
 * If there is no para saved, pass the default value to the algorithm library
 */
int ReadPara(float *p);

/* Save calibration parameters in somewhere that can not lose after power-off.
 * When the system power on next time, need to read this parameters by the function ReadPara() for algorithm initial.
 * If save parameter successfully, return 1, else return -1;
 */
int SavePara(float *p);

/* Delay functon.
 * Parameter nms means delay n ms;
 */
void Delay_Ms(int nms);

/**
 * @brief Convert the sensor coordinate to right-front-up coordinate system;
 */
void acc_coordinate_raw_to_real(int layout, float *in, float *out);

/**
 * @brief Convert the sensor coordinate to right-front-up coordinate system;
 */
void mag_coordinate_raw_to_real(int layout, float *in, float *out);

/*******************************************************************************
* Function Name  : main
* Description    : the main function
*******************************************************************************/





int mm5603_main(void)
{

	/* Initial the acc, mag, and calibrated parameters
	 * if already saved the calibrated offset and radius last time, read it out and init the magOffset and magRadius
	 */
	InitialAlgorithm(mag_smm, mag_hmm);

//	while(1)
//	{
//		/* Get the acc raw data, unit is g*/
//		//acc_raw_data[0] = ;
//		//acc_raw_data[1] = ;
//		//acc_raw_data[2] = ;
//
//		/* convert the coordinate system */
//		acc_coordinate_raw_to_real(A_LAYOUT, acc_raw_data, acc_real_data);
//
//		/* Get the mag raw data, unit is gauss */
//		//mag_raw_data[0] = ;
//		//mag_raw_data[1] = ;
//		//mag_raw_data[2] = ;
//
//		/* Convert the coordinate system */
//		mag_coordinate_raw_to_real(M_LAYOUT, mag_raw_data, mag_real_data);

//		/* Below functions are algorithm interface.
//		 * input acc, mag data into algorithm
//		 * make sure that acc and mag XYZ data meet the right-hand(right-front-up) coordinate system
//		 */
//		MainAlgorithmProcess(acc_real_data, mag_real_data, 1);
//
//		/* Get calibrated mag data */
//		GetCalMag(caliMag);
//
//		/* Calibrated magnetic sensor output, unit is gauss */
//		calMagX = caliMag[0];
//		calMagY = caliMag[1];
//		calMagZ = caliMag[2];
//
//		/* Get orientation vector */
//		GetCalOri(acc_real_data, caliMag, caliOri);
//
//		/* Get the fAzimuth Pitch Roll for the eCompass */
//		fAzimuth  = caliOri[0];
//		fPitch    = caliOri[1];
//		fRoll     = caliOri[2];
//
//		/* Get the accuracy of the algorithm */
//		iAccuracy = GetMagAccuracy();
//


//		/* Sampling interval is 20ms. */
//		Delay_Ms(20);
//	}

	return 1;
}

void mmc5603_arithmetic(void)
{
	/* This variable is used to store the accelerometer and magnetometer raw data.
	 * please NOTE that the data is going to store here MUST has been transmitted
	 * to match the Right-Handed coordinate sytem already.
	 */
	float acc_raw_data[3] = {0.0f};	//accelerometer field vector, unit is g
	float acc_real_data[3] = {0.0f};

	float mag_raw_data[3] = {0.0f};	//magnetic field vector, unit is gauss
	float mag_real_data[3] = {0.0f};

	/* This variable is used to save the calibrated mag sensor XYZ output. */
	float caliMag[3] = {0.0f};

	/* This variable is used to save the calibrated orientation data. */
	float caliOri[3] = {0.0f};


		/* Get the acc raw data, unit is g*/
		signed short tlkXYZ[3];
		extern void SL_SC7A20_GET_DATA(signed short *sl_x_dat,signed short *sl_y_dat,signed short *sl_z_dat);
		SL_SC7A20_GET_DATA(&tlkXYZ[0], &tlkXYZ[1], &tlkXYZ[2]);
		acc_raw_data[0] = tlkXYZ[0]/8192.0;
		acc_raw_data[1] = tlkXYZ[1]/8192.0;
		acc_raw_data[2] = tlkXYZ[2]/8192.0;

//		acc_raw_data[0] = 0;
//		acc_raw_data[1] = 0;
//		acc_raw_data[2] = 1.0;
		/* convert the coordinate system */
		acc_coordinate_raw_to_real(A_LAYOUT, acc_raw_data, acc_real_data);
		/* Get the mag raw data, unit is gauss */
		//mag_raw_data[0] = ;
		//mag_raw_data[1] = ;
		//mag_raw_data[2] = ;
		MMC5603_GetData(mag_raw_data);
		/* Convert the coordinate system */
		mag_coordinate_raw_to_real(M_LAYOUT, mag_raw_data, mag_real_data);

		/* Below functions are algorithm interface.
		 * input acc, mag data into algorithm
		 * make sure that acc and mag XYZ data meet the right-hand(right-front-up) coordinate system
		 */


		MainAlgorithmProcess(acc_real_data, mag_real_data, 1);


		/* Get calibrated mag data */
		GetCalMag(caliMag);

		/* Calibrated magnetic sensor output, unit is gauss */
		calMagX = caliMag[0];
		calMagY = caliMag[1];
		calMagZ = caliMag[2];

		/* Get orientation vector */
		GetCalOri(acc_real_data, caliMag, caliOri);

		/* Get the fAzimuth Pitch Roll for the eCompass */
		fAzimuth  = caliOri[0];
		fPitch    = caliOri[1];
		fRoll     = caliOri[2];

		/* Get the accuracy of the algorithm */
		iAccuracy = GetMagAccuracy();

		//printf("fAzimuth == %d,fPitch == %d,fRoll == %d",fAzimuth,fPitch,fRoll);
		tlkapi_trace(TLKDRV_GEO_MMC5603_DBG_FLAG, TLKDRV_GEO_MMC5603_DBG_SIGN,
			"fAzimuth == %d,fPitch == %d,fRoll == %d",fAzimuth,fPitch,fRoll);
		//DBG_DIRECT("acc_real_data[0] == %f,acc_real_data[1] == %f,acc_real_data[2] == %f,mag_real_data[0] == %f,mag_real_data[1] == %f,mag_real_data[2] == %f,fAzimuth == %f",
		//acc_real_data[0],acc_real_data[1],acc_real_data[2],mag_real_data[0],mag_real_data[1],mag_real_data[2],fAzimuth);
		/* Sampling interval is 20ms. */
		//Delay_Ms(20);

}


/* Read saved calibration parameters. These para is saved in somewhere that can not disappear when power-off
 * ReadPara function should be create by customer.If customer can not implement this function, should comment it out
 * If there is no parameter saved, ReadPara() should return -1, else return 1;
 * If there is no para saved, pass the default value to the algorithm library
 */
int ReadPara(float *p)
{
	int i;
	float sp[4] = {0.0f,0.0f,0.0f,0.5f};

	/*
	.
	. Need to be implemented by user.
	.
	*/

	for(i=0;i<4;i++){
		p[i] = sp[i];
	}
	return 1;
}

/* Save calibration parameters in somewhere that can not lose after power-off.
 * When the system power on next time, need to read this parameters by the function ReadPara() for algorithm initial.
 * If save parameter successfully, return 1, else return -1;
 */
int SavePara(float *p)
{
	/*
	.
	. Need to be implemented by user.
	.
	*/

	return 1;
}


/* Convert the sensor coordinate to right-front-up coordinate system;
 */
void acc_coordinate_raw_to_real(int layout, float *in, float *out)
{
	switch (layout) {
	case 0:
		out[0] =  in[0];
		out[1] =  in[1];
		out[2] =  in[2];
		break;
	case 1:
		out[0] = -in[1];
		out[1] =  in[0];
		out[2] =  in[2];
		break;
	case 2:
		out[0] = -in[0];
		out[1] = -in[1];
		out[2] =  in[2];
		break;
	case 3:
		out[0] =  in[1];
		out[1] = -in[0];
		out[2] =  in[2];
		break;
	case 4:
		out[0] =  in[1];
		out[1] =  in[0];
		out[2] = -in[2];
		break;
	case 5:
		out[0] = -in[0];
		out[1] =  in[1];
		out[2] = -in[2];
		break;
	case 6:
		out[0] = -in[1];
		out[1] = -in[0];
		out[2] = -in[2];
		break;
	case 7:
		out[0] =  in[0];
		out[1] = -in[1];
		out[2] = -in[2];
		break;
	default:
		out[0] =  in[0];
		out[1] =  in[1];
		out[2] =  in[2];
		break;
	}
}
/* Convert the sensor(MMC3630KJ) coordinate to right-front-up coordinate system;
 */
void mag_coordinate_raw_to_real(int layout, float *in, float *out)
{
	switch (layout) {
	case 0:
		out[0] =  in[0];
		out[1] =  in[1];
		out[2] =  in[2];
		break;
	case 1:
		out[0] = -in[1];
		out[1] =  in[0];
		out[2] =  in[2];
		break;
	case 2:
		out[0] = -in[0];
		out[1] = -in[1];
		out[2] =  in[2];
		break;
	case 3:
		out[0] =  in[1];
		out[1] = -in[0];
		out[2] =  in[2];
		break;
	case 4:
		out[0] =  in[1];
		out[1] =  in[0];
		out[2] = -in[2];
		break;
	case 5:
		out[0] = -in[0];
		out[1] =  in[1];
		out[2] = -in[2];
		break;
	case 6:
		out[0] = -in[1];
		out[1] = -in[0];
		out[2] = -in[2];
		break;
	case 7:
		out[0] =  in[0];
		out[1] = -in[1];
		out[2] = -in[2];
		break;
	default:
		out[0] =  in[0];
		out[1] =  in[1];
		out[2] = -in[2];
		break;
	}
}



#endif //#if TLKDRV_GSENSOR_MMC5603_ENABLE

