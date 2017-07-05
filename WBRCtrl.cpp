#include "WBRCtrl.h"


//
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

/// <summary>
/// ���C���֐�
/// </summary>
/// <returns></returns>
int WBRCtrl::main()
{
	Direction direct = Left;

	if (CheckBattery(START_BATTERY))//�N���d���ɒB���Ă��Ȃ����Ƃ��m�F
	{
		delay(CHARGE_DELAY_TIME);//��d
	}

	while (CheckBattery(BAD_BATTERY))//�N���Œ�d���łȂ������m�F
	{
		SetPwmMove();
		while (IsEdge() == true)
		{
			if (IsCorner() == true)
			{
				Turn(deg180,direct);

				direct = direct == Right ? Left : Right;//���̊p�̉�]���������肷��B

				break;//continue�ł��������ǋC����"Break"
			}
			FloorDirectionTurning();//���[�^�[��PWM����~��ԂŋA���Ă���̂Œ���
		}
		
		//�A�җp�̊֐��������K�v������܂��B

	}

	/*�ȉ��v���Ƃ���ɉ�����������ݒ肷��R�[�h�@�_���I�ɔj�]�������߃p�[�W
	//�v���J�n
	int width = MeasureDistanceByRoll();// / SPINCOUNT_BODY_LENGTH;//�������̌v���l/�{�̈�̕��̉�]��
	Turn(deg90, Left);
	int	height = MeasureDistanceByRoll()/ SPINCOUNT_BODY_LENGTH;//�c�����̌v���l/�{�̈�̕��̉�]��
	Turn(deg90, Left);
	x_location = 0;
	y_location = height;

	while (y_location <= SPINCOUNT_BODY_LENGTH);
	{
		Move();
		RolltoStopByCount(width,width);
		Stop();
		Turn(deg180, Left);
	}

	*/
}



/// <summary>
/// �����o���z���C�g�{�[�h�̒[�ɗ������Ƃ����m
/// </summary>
/// <returns>�[���ǂ���</returns>
bool WBRCtrl::IsEdge()
{
	int Edge = false;

	int FRcensor = digitalRead(PinFRSensor);
	int FLcensor = digitalRead(PinFLSensor);
	int FCcensor = digitalRead(PinFCSensor);

	FCcensor = (PinFCSensor == 0) ? 0 : FCcensor;

	if (FRcensor == FLOOR_NOTEXIST || FLcensor == FLOOR_NOTEXIST || FCcensor == FLOOR_NOTEXIST)//�ǂꂩ��ɃZ���T�[�Ɉ�������������
	{
		if (FRcensor == FLOOR_EXIST || FLcensor == FLOOR_EXIST)
		{
			//FloorDirectionTurning();
			return Edge = true;
		}
	}

	return Edge;

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

	if (SideRcensor == FLOOR_EXIST || SideLcensor == FLOOR_EXIST)
	{
			return true;
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
/// <summary>
/// �z���C�g�{�[�h�㉺���E�́A�����烋���o����яo�����Ƃ����Ƃ��ɉ�]�����Ă��Ƃɖ߂�֐�
/// </summary>
void WBRCtrl::FloorDirectionTurning()
{
	int Rcensor = digitalRead(PinFRSensor);
	int Lcensor = digitalRead(PinFLSensor);

	if (Rcensor == FLOOR_EXIST)
	{

		while (digitalRead(PinFRSensor) != digitalRead(PinFLSensor))
		{
			SetPwmTurn(Left);
		}
	}

	if (Lcensor == FLOOR_EXIST)
	{
		while (digitalRead(PinFRSensor) != digitalRead(PinFLSensor))
		{
			SetPwmTurn(Right);
		}
	}

	SetPwmStop();

	return;

}
/*
�����������p

*/
void WBRCtrl::SetPwmTurn(Direction dic)
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
void WBRCtrl::SetPwmMove()
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
void WBRCtrl::SetPwmStop()
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
void WBRCtrl::Turn(Angle angle, Direction direct)
{

	if (angle == deg90)
	{
		for (int i = 0; i < 5; i++)	//5�͉��̒l
		{
			SetPwmTurn(direct);

			delay(SPIN_DELAY_TIME);
		}
		return;
	}

	Turn(deg90, direct);

	SetPwmMove();
	StopRollByCount(1, 1);
	SetPwmStop();

	Turn(deg90, direct);

	if (direct == NEXT_TURN_LEFT)
	{
		direct = (Direction)NEXT_TURN_RIGHT;
	}

	if (direct == NEXT_TURN_RIGHT)
	{
		direct = (Direction)NEXT_TURN_LEFT;
	}
	return;

}

void WBRCtrl::BackHome()
{
	int magnet_sencer = 1;

	
}

/// <summary>
/// �w��̉�]����]���s�킹��֐�
/// </summary>
/// <param name="RspinCount_TargetCount">�E���[�^�[�̎w���]��</param>
/// <param name="LspinCount_TargetCount">�����[�^�[�̎w���]��</param>
void WBRCtrl::StopRollByCount(int RspinCount_TargetCount, int LspinCount_TargetCount)
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
/// �p�܂ł̋���(�^�C���̉�]��)���v������֐�
/// </summary>
/// <returns>�^�C���̉�]������</returns>
int WBRCtrl::MeasureDistanceByRoll()
{
	SetPwmMove();
	int spinCount = 0;
	for (int RspinCount,LspinCount = 0;;)
	{
		if (IsCorner())
		{
			SetPwmStop();
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
