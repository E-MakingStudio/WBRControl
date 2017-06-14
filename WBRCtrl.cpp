#include "WBRCtrl.h"



WBRCtrl::WBRCtrl
(
	int _PinRmotorPWMf,
	int	_PinLmotorPWMf,
	int _PinRmotorPWMb,
	int _PinLmotorPWMb,
	int _PinReadBattery,
	int	_PinFRSensor,
	int	_PinFLSensor,
	int	_PinFCSensor,
	int	_PinSideRSensor,
	int	_PinSideLSensor,
	int	_PinMagnetSensor,
	int	_PinRRotary_Encoder,
	int	_PinLRotary_Encoder

)
{
	PinRmotorPWMf = _PinRmotorPWMf;
	PinLmotorPWMf = _PinLmotorPWMf;
	PinRmotorPWMb = _PinRmotorPWMb;
	PinLmotorPWMb = _PinLmotorPWMb;
	PinReadBattery = _PinReadBattery;
	PinFRSensor = _PinFRSensor;
	PinFLSensor = _PinFLSensor;
	PinFCSensor = _PinFCSensor;
	PinSideRSensor = _PinSideRSensor;
	PinSideLSensor = _PinSideLSensor;
	PinMagnetSensor = _PinMagnetSensor;
	PinRRotary_Encoder = _PinRRotary_Encoder;
	PinLRotary_Encoder = _PinLRotary_Encoder;


}
void WBRCtrl::PinInitialization()
{
	pinMode(PinRmotorPWMf, OUTPUT);
	pinMode(PinLmotorPWMf, OUTPUT);
	pinMode(PinRmotorPWMb, OUTPUT);
	pinMode(PinLmotorPWMb, OUTPUT);
	pinMode(PinReadBattery, INPUT);
	pinMode(PinFLSensor, INPUT);
	pinMode(PinFCSensor, INPUT);
	pinMode(PinFRSensor, INPUT);
	pinMode(PinSideRSensor, INPUT);
	pinMode(PinSideLSensor, INPUT);
	pinMode(PinMagnetSensor, INPUT);
	pinMode(PinRRotary_Encoder, INPUT);
	pinMode(PinLRotary_Encoder, INPUT);
}

/*
メイン関数
*/
int WBRCtrl::main()
{
	Direction direct;

	if (!CheckBattery(START_BATTERY))
	{
		delay(CHARGE_DELAY_TIME);
	}

	//電池がなくなるまでループ
	while (!CheckBattery(BAD_BATTERY))
	{
		while (FloorCheck() == FLOOR_EXIST)
		{
			Move();
		}
		if (CornerCheck(&direct) == CORNER_EXIST)
			break;
		else
		{
			Turn(deg180, &direct);
		}
	}
	BackHome(&direct);

}

/*
ルンバがWBの端に到達したことを判定する
*/
int WBRCtrl::FloorCheck()
{
	int nRet = FLOOR_EXIST;

	int FRcensor = digitalRead(PinFRSensor);
	int FLcensor = digitalRead(PinFLSensor);
	int FCcensor = digitalRead(PinFCSensor);

	FCcensor = (PinFCSensor == 0) ? 0 : FCcensor;

	if (FRcensor + FLcensor + FCcensor >= FLOOR_NOTEXIST)
	{
		if (FRcensor != FLcensor)
		{
			FloorDirectionTurning();

		}
		nRet = FLOOR_NOTEXIST;
	}
	return nRet;

}

/*

*/
int WBRCtrl::CornerCheck(Direction *direct)
{
	int nRet = CORNER_NOTEXIST;
	int SideRcensor = digitalRead(PinSideRSensor);
	int SideLcensor = digitalRead(PinSideLSensor);

	static int firstCorner = 0;

	if (SideRcensor + SideLcensor >= FLOOR_NOTEXIST)
	{
		if (firstCorner % 2 == 0)
		{
			if (SideLcensor == FLOOR_NOTEXIST)
			{
				*direct = (Direction)NEXT_TURN_RIGHT;
			}

			else
			{
				*direct = (Direction)NEXT_TURN_LEFT;
			}

		}
		else
		{
			nRet = CORNER_EXIST;
		}
		firstCorner++;
	}

	return nRet;
}

/*
バッテリー残量が指定の値以上かを判する関数
batteryTartgetValue:バッテリーの残量(電圧)の閾値
*/
bool WBRCtrl::CheckBattery(int batteryTartgetValue)
{
	return analogRead(PinReadBattery) <= batteryTartgetValue ? false : true;
}

void WBRCtrl::FloorDirectionTurning()
{
	int Rcensor = digitalRead(PinFRSensor);
	int Lcensor = digitalRead(PinFLSensor);

	if (Rcensor == FLOOR_EXIST)
	{

		while (digitalRead(PinFRSensor) != digitalRead(PinFLSensor))
		{
			FloorTurning(Left);
		}
	}

	if (Lcensor == FLOOR_EXIST)
	{
		while (digitalRead(PinFRSensor) != digitalRead(PinFLSensor))
		{
			FloorTurning(Right);
		}
	}

	Stop();

	return;

}
/*
床方向調整用

*/
void WBRCtrl::FloorTurning(Direction dic)
{
	if (dic == Right)
	{
		analogWrite(PinLmotorPWMf, TURN_PWM);
		analogWrite(PinLmotorPWMb, 0);
		analogWrite(PinRmotorPWMf, 0);
		analogWrite(PinRmotorPWMb, TURN_PWM);
		return;
	}
	analogWrite(PinLmotorPWMf, 0);
	analogWrite(PinLmotorPWMb, TURN_PWM);
	analogWrite(PinRmotorPWMf, TURN_PWM);
	analogWrite(PinRmotorPWMb, 0);

	return;
}

/*
前進
*/
void WBRCtrl::Move()
{
	analogWrite(PinLmotorPWMf, LEFT_PWM_SPEED);
	analogWrite(PinLmotorPWMb, 0);

	analogWrite(PinRmotorPWMf, RIGHT_PWM_SPEED);
	analogWrite(PinRmotorPWMb, 0);


	return;
}
/*
停止
*/
void WBRCtrl::Stop()
{
	analogWrite(PinLmotorPWMb, 0);
	analogWrite(PinLmotorPWMf, 0);

	analogWrite(PinRmotorPWMb, 0);
	analogWrite(PinRmotorPWMf, 0);

	return;
}

/*
方向転換
angle:回転する角度
direct:回転する方向
*/
void WBRCtrl::Turn(Angle angle, Direction *direct)
{

	if (angle == deg90)
	{
		for (int i = 0; i < 5; i++)	//5は仮の値
		{
			FloorTurning(*direct);

			delay(SPIN_DELAY_TIME);
		}
		return;
	}

	Turn(deg90, direct);

	Move();
	RolltoStopByCount(1, 1);
	Stop();

	Turn(deg90, direct);

	if (*direct == NEXT_TURN_LEFT)
	{
		*direct = (Direction)NEXT_TURN_RIGHT;
	}

	if (*direct == NEXT_TURN_RIGHT)
	{
		*direct = (Direction)NEXT_TURN_LEFT;
	}
	return;

}
/*

*/
void WBRCtrl::BackHome(Direction *direct)
{
	int magnet_sencer = 1;

	while (magnet_sencer == 0)
	{

		*direct = (Direction)NEXT_TURN_RIGHT;								//turn関数を固定

																// ホワイトボード右側の端にぶつかるまで前進
		while (FloorCheck() == FLOOR_EXIST)
		{

			Move();

		}

		Turn(deg90, direct);           						// 旋回関数を用いて「その場」で時計回りに90度回転

		delay(MGNET_SERACH_DELAY_TIME);						//　磁力感知がすぐできない場合を考慮  

	}
}

/*
指定の回転を行ったら回転を止める関数
*/
void WBRCtrl::RolltoStopByCount(int RspinCount_TargetCount, int LspinCount_TargetCount)
{
	RspinCount_TargetCount, LspinCount_TargetCount *= SPINCOUNT_TARGETVALUE;

	for (int RspinCount, LspinCount = 0; RspinCount_TargetCount >= RspinCount && LspinCount_TargetCount >= LspinCount;)
	{
		if (digitalRead(PinRRotary_Encoder) == 1)
		{
			RspinCount++;
		}
		if (digitalRead(PinLRotary_Encoder) == 1)
		{
			LspinCount++;
		}
	}
}
