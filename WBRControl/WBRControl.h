#ifndef WBRControl_H_
#define WBRControl_H_

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
#define RIGHT_PWM_SPEED		179		// 前進時右モーターの回転数
#define LEFT_PWM_SPEED		234		// 前進時左モーターの回転数

//　StartBatteryCheck
#define START_BATTERY			3.5		//十分にバッテリーが充電されているときの電圧
#define CHARGE_DELAY_TIME		1000		// 充電待機時間

// Turn90deg
#define SPIN_DELAY_TIME		500		// 回転時の待機時間
#define NEXT_TURN_RIGHT		0		//回転方向を右回りに
#define NEXT_TURN_LEFT		1		//回転方向を左回りに

//　BackHome
#define MGNET_SERACH_DELAY_TIME		500		// 磁力感知のための待機時間

//　Rotary_Encoder
#define ROTARY_ENCODER_SPIN		1		//タイヤの回転数を指定
#define SPINCOUNT_TARGETVALUE		4		//一回転に必要なパルス数

#include "arduino.h"


class WBRControl
{
	private:
		int pinRightMotorPWMGo;		// 左モーターのＰＷＭ正接出力ピン(デジタル)
		int pinRightMotorPWMBack;		// 左モーターのＰＷＭ逆接出力ピン(デジタル)
		int pinLeftMotorPWMGo;		// 右モーターのＰＷＭ正接出力ピン(デジタル)
		int pinLeftMotorPWMBack;		// 右モーターのＰＷＭ逆接出力ピン(デジタル)
		int pinReadBattery;		// 電圧測定用ピン(センサ番号仮)
		int pinFrontRightSensor;		// 前方右センサのピン番号(デジダル)
		int pinFrontLeftSensor;		// 前方左センサのピン番号（デジタル）
		int pinFrontCenterSensor;		// 前方中央センサのピン番号（存在しない場合は省く）（デジタル）
		int pinSideRightSensor;		// 右側面センサのピン番号（デジタル）
		int pinSideLeftSensor;		// 左側面センサのピン番号（デジタル）
		int pinMagnetSensor;		// 給電所で磁力を検知するためのセンサのピン番号(そもそもセンサが存在するのだろうか？)(センサ番号仮)
		int pinRightRotary_Encoder;		//右モーターのロータリーエンコーダ出力ピン(4月21日現在A3ピン)
		int pinLeftRotary_Encoder;		//左モーターのロータリーエンコーダ出力ピン(4月21日現在A2ピン)

	public:
		WBRControl(
		int pinRightMotorPWMGo,
		int pinRightMotorPWMBack,
		int pinLeftMotorPWMGo,
		int pinLeftMotorPWMBack,
		int pinReadBattery,
		int pinFrontRightSensor,
		int pinFrontLeftSensor,
		int pinFrontCenterSensor,
		int pinSideRightSensor,
		int pinSideLeftSensor,
		int pinMagnetSensor,
		int pinRightRotary_Encoder,
		int pinLeftRotary_Encoder
	);

	int WBRMain();
	void pinInit();
	int FloorCheck();
	int CornerCheck(int *turn);
	void FloorDirectionTurning();
	void FloorTurningRight();
	void FloorTurningLeft();
	void GoForward();
	void StartupBatteryCheck();
	void Turn180deg(int *turn);
	void Turn90deg(int *turn);
	void BackHome(int *turn);
	void Rotary_Encoder(int RightSpinCount_TargetValue, int LeftSpinCount_TargetValue);

};

#endif
