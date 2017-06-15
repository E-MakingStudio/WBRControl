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
		if (IsCorner(&direct))
			break;
		else
		{
			Turn(deg180, &direct);
		}
	}
	BackHome(&direct);

}



/// <summary>
/// �����o���z���C�g�{�[�h�̒[�ɗ������Ƃ����m
/// </summary>
/// <returns>�[���ǂ���</returns>
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

/// <summary>
/// �p���ǂ������m�F����֐�
/// </summary>
/// <param name="direct">��]����</param>
/// <returns>�p�ł����[true]�łȂ����[false]</returns>
bool WBRCtrl::IsCorner()
{
	int SideRcensor = digitalRead(PinSideRSensor);
	int SideLcensor = digitalRead(PinSideLSensor);

	static int firstCorner = 0;

	if (SideRcensor + SideLcensor >= FLOOR_NOTEXIST)
	{
		if (firstCorner % 2 == 0)
		{
			if (SideLcensor == FLOOR_NOTEXIST)
			{
				//*direct = (Direction)NEXT_TURN_RIGHT;
			}

			else
			{
				//*direct = (Direction)NEXT_TURN_LEFT;
			}

		}
		else
		{
			return true;
		}
		firstCorner++;
	}
	
	return false;
}


///<summary>�o�b�e���[�̎c�ʊm�F</summary>
///<param name='batteryTartgetValue'>�o�b�e���[�d���l </param>
///<return>�o�b�e���[�d��������̒l�𖞂����Ă��邩�ǂ���</return>
///
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

/// <summary>
/// ���[�^�[�̉�]����O�i��Ԃɐݒ肷��
/// </summary>
void WBRCtrl::Move()
{
	analogWrite(PinLmotorPWMf, LEFT_PWM_SPEED);
	analogWrite(PinLmotorPWMb, 0);

	analogWrite(PinRmotorPWMf, RIGHT_PWM_SPEED);
	analogWrite(PinRmotorPWMb, 0);


	return;
}
/// <summary>
/// ���[�^�[�̉�]���~��Ԃɐݒ肷��
/// </summary>
void WBRCtrl::Stop()
{
	analogWrite(PinLmotorPWMb, 0);
	analogWrite(PinLmotorPWMf, 0);

	analogWrite(PinRmotorPWMb, 0);
	analogWrite(PinRmotorPWMf, 0);

	return;
}

/// <summary>
/// ��]���s���֐�
/// </summary>
/// <param name="angle">�p�x[90deg,180deg]</param>
/// <param name="direct">����[Right,Left]</param>
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

/// <summary>
/// �w��̉�]����]���s�킹��֐�
/// </summary>
/// <param name="RspinCount_TargetCount">�E���[�^�[�̎w���]��</param>
/// <param name="LspinCount_TargetCount">�����[�^�[�̎w���]��</param>
void WBRCtrl::RolltoStopByCount(int RspinCount_TargetCount, int LspinCount_TargetCount)
{
	RspinCount_TargetCount, LspinCount_TargetCount *= SPINCOUNT_TARGETVALUE;//���]������̃p���X���Ɖ�]���������ĕK�v�ȃp���X�����Z�o

	for (int RspinCount, LspinCount = 0; RspinCount_TargetCount >= RspinCount && LspinCount_TargetCount >= LspinCount;)//�K��̃p���X���p���X�����U�����܂ŌJ��Ԃ�
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

/// <summary>
/// �p�܂ł̋������v������֐�
/// </summary>
/// <returns>�^�C���̉�]������</returns>
int WBRCtrl::MeasureDistanceByRoll()
{
	Move();
	int spinCount = 0;
	for (int RspinCount,LspinCount = 0;;)
	{
		if (IsCorner())
		{
			Stop();
			spinCount = RspinCount < LspinCount ? RspinCount : LspinCount;
			break;
		}
		if (digitalRead(PinRRotary_Encoder) == 1)
		{
			RspinCount++;
		}
		if (digitalRead(PinLRotary_Encoder) == 1)
		{
			LspinCount++;
		}
	}
	return spinCount/SPINCOUNT_TARGETVALUE;

}
