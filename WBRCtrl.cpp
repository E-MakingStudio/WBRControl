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
���C���֐�
*/
int WBRCtrl::main()
{
	Direction direct;

	if (!CheckBattery(START_BATTERY))
	{
		delay(CHARGE_DELAY_TIME);
	}

	//�d�r���Ȃ��Ȃ�܂Ń��[�v
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
�����o��WB�̒[�ɓ��B�������Ƃ𔻒肷��
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
�o�b�e���[�c�ʂ��w��̒l�ȏォ�𔻂���֐�
batteryTartgetValue:�o�b�e���[�̎c��(�d��)��臒l
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
�����������p

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
�O�i
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
��~
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
�����]��
angle:��]����p�x
direct:��]�������
*/
void WBRCtrl::Turn(Angle angle, Direction *direct)
{

	if (angle == deg90)
	{
		for (int i = 0; i < 5; i++)	//5�͉��̒l
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

		*direct = (Direction)NEXT_TURN_RIGHT;								//turn�֐����Œ�

																// �z���C�g�{�[�h�E���̒[�ɂԂ���܂őO�i
		while (FloorCheck() == FLOOR_EXIST)
		{

			Move();

		}

		Turn(deg90, direct);           						// ����֐���p���āu���̏�v�Ŏ��v����90�x��]

		delay(MGNET_SERACH_DELAY_TIME);						//�@���͊��m�������ł��Ȃ��ꍇ���l��  

	}
}

/*
�w��̉�]���s�������]���~�߂�֐�
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
