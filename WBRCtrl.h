#pragma once

#ifndef WBRCTRL_H_
#define WBRCTRL_H_
// WBRMain
#define BAD_BATTERY			0.5		//	バッテリーが無くなったの電圧
//　FloorCheck
// (非常に残念なことに、床があるときにセンサが返す値は０[LOW]です)
#define FLOOR_EXIST			0		// 床あり
#define FLOOR_NOTEXIST		1		// 床なし
//　CornerCheck
#define CORNER_EXIST		1		// 角あり
#define CORNER_NOTEXIST		0		// 角なし
//　FloorDirectionturning
#define TURN_PWM				145		// モーターの回転数
#define FLOOR_DELAY_TIME 		100		// 調整時の待機時間
//　GoForward
#define RIGHT_PWM_SPEED		200		// 前進時右モーターの回転数
#define LEFT_PWM_SPEED		200		// 前進時左モーターの回転数
//　StartBatteryCheck
#define START_BATTERY			3.5		//十分にバッテリーが充電されているときの電圧
#define CHARGE_DELAY_TIME		1000		// 充電待機時間\
// Turn90deg
#define SPIN_DELAY_TIME		500		// 回転時の待機時間
#define NEXT_TURN_RIGHT		0		//回転方向を右回りに
#define NEXT_TURN_LEFT		1		//回転方向を左回りに
//　BackHome
#define MGNET_SERACH_DELAY_TIME		500		// 磁力感知のための待機時間
//　Rotary_Encoder
#define ROTARY_ENCODER_SPIN		1		//タイヤの回転数を指定
#define SPINCOUNT_TARGETVALUE		28		//一回転に必要なパルス数
//	
#define SPINCOUNT_BODY_LENGTH	  114514			//一本体分移動する為のパルス数
#define TURNCOUNT_TO_VERTICAL	1919810			//90回転するために必要な回転数

#define DEBUG

#include <arduino.h>
/**
右、左の方向の列挙型
**/
enum Direction
{
	Right = 0, Left = 1, Forward = 2, Back = 3,
};
/**
回転の角度の列挙型
**/
enum Angle
{
	deg90, deg180
};

class WBRCtrl
{
private:
	int PinRmotorPWMf;	//右モーターのPWM正接出力ピン(2)
	int	PinLmotorPWMf;	//左モーターのPWM正接出力ピン(2)
	int PinRmotorPWMb;	//右モーターのPWM逆接出力ピン(2)
	int PinLmotorPWMb;	//左モーターのPWM逆接出力ピン(2)
	int PinReadBattery;
	int	PinFRSensor;
	int	PinFLSensor;
	int	PinFCSensor;
	int	PinSideRSensor;
	int	PinSideLSensor;
	int	PinMagnetSensor;
	int	PinRRotary_Encoder;
	int	PinLRotary_Encoder;

public:
	WBRCtrl
	(
		int PinRmotorPWMf,
		int	PinLmotorPWMf,
		int PinRmotorPWMb,
		int PinLmotorPWMb,
		int PinReadBattery,
		int	PinFRSensor,
		int	PinFLSensor,
		int	PinFCSensor,
		int	PinSideRSensor,
		int	PinSideLSensor,
		int	PinMagnetSensor,
		int	PinRRotary_Encoder,
		int	PinLRotary_Encoder
	);

	int	main();
	void PinInitialization();
	bool IsEdge();
	bool IsCorner();
	bool CheckBattery(int batteryTartgetValue);
	void FloorDirectionTurning();
	void SetPwmTurn(Direction dic);
	void SetPwmMove(Direction Rdic,Direction Ldic);
	void SetPwmStop();
	void Turn(Angle angle, Direction direct);
	void BackHome();
	void MoveByCount(float RSpinCount_TargetValue, float LSpinCount_TargetValue);
	int MeasureDistanceByRoll();
};


#endif


