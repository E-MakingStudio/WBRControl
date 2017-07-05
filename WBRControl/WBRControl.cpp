#include "arduino.h"
#include "WBRControl.h"

/**
 * WBRControl
 * コンストラクタ
 */
WBRControl::WBRControl(
	int _pinRightMotorPWMGo,
    int _pinRightMotorPWMBack,
    int _pinLeftMotorPWMGo,
    int _pinLeftMotorPWMBack,
    int _pinReadBattery,
    int _pinFrontRightSensor,
    int _pinFrontLeftSensor,
    int _pinFrontCenterSensor,
    int _pinSideRightSensor,
    int _pinSideLeftSensor,
    int _pinMagnetSensor,
    int _pinRightRotary_Encoder,
	int _pinLeftRotary_Encoder
	){
		
		pinRightMotorPWMGo 		= _pinRightMotorPWMGo;
    	pinRightMotorPWMBack 	= _pinRightMotorPWMBack;
    	pinLeftMotorPWMGo 		= _pinLeftMotorPWMGo;
    	pinLeftMotorPWMBack 	= _pinLeftMotorPWMBack;
    	pinReadBattery 			= _pinReadBattery;
    	pinFrontRightSensor 	= _pinFrontRightSensor;
    	pinFrontLeftSensor 		= _pinFrontLeftSensor;
    	pinFrontCenterSensor 	= _pinFrontCenterSensor;
    	pinSideRightSensor 		= _pinSideRightSensor;
    	pinSideLeftSensor 		= _pinSideLeftSensor;
    	pinMagnetSensor 		= _pinMagnetSensor;
		pinRightRotary_Encoder	= _pinRightRotary_Encoder;
		pinLeftRotary_Encoder	= _pinLeftRotary_Encoder;
}

/** 
 * pinInit
 * ピンの初期化
 */
void WBRControl::pinInit(){

	pinMode(pinRightMotorPWMGo, OUTPUT);
	pinMode(pinRightMotorPWMBack, OUTPUT);
	pinMode(pinLeftMotorPWMGo, OUTPUT);
	pinMode(pinLeftMotorPWMBack, OUTPUT);
	pinMode(pinReadBattery, INPUT);
	pinMode(pinFrontRightSensor, INPUT);
	pinMode(pinFrontLeftSensor, INPUT);
	pinMode(pinFrontCenterSensor, INPUT);
	pinMode(pinSideRightSensor, INPUT);
	pinMode(pinSideLeftSensor, INPUT);
	pinMode(pinMagnetSensor, INPUT);
	pinMode(pinRightRotary_Encoder, INPUT);
	pinMode(pinLeftRotary_Encoder, INPUT);
	
}

/** 
 * WBRmain
 * WBRのメイン
 */
int WBRControl::WBRMain(){

	Serial.println("In Main.");	
	
	int turn;		// // 曲がった方向を記録する変数を宣言	
	
	StartupBatteryCheck(); //起動前電池チェック
	
	// 電池がなくなるまで帰還せずにループする
	// 電池が読み込めないときは下の文で充電が十分と仮定する
	//while( analogRead(pinReadBattery) >= BAD_BATTERY  ){
	while( 6.0 >= BAD_BATTERY  ){	

		//床があるかぎり前進
		while( FloorCheck() == FLOOR_EXIST){	

			GoForward();			

		}
		
		// 床がなければ角判定する、角があればループをブレイクして帰還する
		if( CornerCheck(&turn) == CORNER_EXIST ){

			break;	

		}

		// 角がなければ曲がり、最初に戻ってループする	
		else{

			Turn180deg(&turn);		

		}
		
	}

	BackHome(&turn);					//帰還する
	Serial.println("End Main.");

}	

/**
 * 床判定_FloorCheck 
 * WBRルンバがホワイトボードの端に到達した（それ以上WBRが前進出来ない状態）かどうかを判定する
 * 床が無いことを判別するための関数のため、前方デジタルセンサーの値がすべて1（＝床がある）のときはスルーされることに注意
 */
int WBRControl::FloorCheck(){

	Serial.println("In FloorCheck.");

	int nRet = FLOOR_EXIST;     // WBRの前方に床がある状態で初期化
	
    // 各前方デジタルセンサーの値を関数中の変数として定義
	// (床があるときにセンサが返す値は０[LOW]です)
	int right           = digitalRead(pinFrontRightSensor);        // 前方右センサの状態を取得
	int left            = digitalRead(pinFrontLeftSensor);         // 前方左センサの状態を取得
	int center          = digitalRead(pinFrontCenterSensor);       // 前方中央センサの状態を取得（存在しない場合は省く）
	//int s_right         = digitalRead(pinSideRightSensor);          // 側面右センサの状態を取得
	//int s_left          = digitalRead(pinSideLeftSensor);          // 側面左センサの状態を所得

	// 
	
    /**
     * 前方のデジタルセンサの内、どれか1つでも床が無いことを認識しているか確認
     * (中央センサが存在しない/使用しないときはピンの値を読ませずに常に反応していると認定させる)
     */

	if(pinFrontCenterSensor == 0){
	center = 0;
	}

	if(right + left + center >= FLOOR_NOTEXIST){
        
        /**
         * 重力によって進行方向が地面側へ曲がっていないかを確認（ボードの下端で前方センサが反応しているかどうか）
         * もしそれで反応していたらこの時に進行方向の調整が必要…？
         * この判定をしないと床方向調整で下端に対して垂直になる気がする ←その時点で磁力が足りていないので,ハード的な磁石の変更が必要になるから考えなくていいかと
    	 * if(s_right * s_left != 0){}
         */
    	
        /**
         * 前方右センサか前方左センサの片方だけ、床が無いことを認識している、要は力によって進行方向が地面側へ曲がっていないか
         * （＝WBRの直進方向がホワイトボードの外枠に垂直ではない）かを確認
         */
		if(right != left){

			FloorDirectionTurning();     // 床方向調整を行い、WBRの向きをホワイトボードの外枠に対して垂直に修正

		}
    	
		 nRet = FLOOR_NOTEXIST;           // WBRの前方に床が無い状態に変更
		
		
	}
	Serial.println("Out FloorCheck.");
	return nRet;
}

/**
 * 角判定 CornerCheck
 * WBRルンバがホワイトボードの端から端まで清掃しきった（スタート地点からWBRがボード反対の角まで辿り着いた）かどうかを判定する
 * 床判定からFalse（＝床が無い）とメイン文に返って来た時にこの関数が呼びだされることが前提
 * 重力によるずり下がりを考えると、必ずしもボードの端にたどり着いてからそのまま真っ直ぐ進めるとは限らないため
 * 旋回関数中に上端に達した場合、それを記憶しておくフラグを立てておく必要があるかもしれないです
 * ただずり下がりを常に考慮すると、一番最初のボードの下端走行時ですらまともに走れないことになりますが…
 */
int WBRControl::CornerCheck(int *turn){

	Serial.println("In CornerCheck.");

	int nRet = CORNER_NOTEXIST;						// 角にぶつかっていない状態で初期化
	int S_right = digitalRead(pinSideRightSensor);	// 側面右センサの状態を取得
	int S_left = digitalRead(pinSideLeftSensor);		// 側面左センサの状態を取得

    /**
     * 一度でも角判定がされたこと（ボードの下端の角を認識済か）を関数内で保存する
     * (グローバル関数にするか迷ったがとりあえず静的ローカル変数で宣言)
     */
	static int firstCorner= 0;                    

    // 側面のセンサーが壁に当たっている（＝床がない）ことを検知
	if(S_right + S_left >= FLOOR_NOTEXIST){

	    // 初めて角にぶつかった場合
		if(firstCorner%2 == 0){

			// 左側面のセンサーが床が無いと反応した場合
			if(S_left == FLOOR_NOTEXIST){

				*turn = NEXT_TURN_RIGHT;           					// 旋回関数に右旋回から旋回をさせるように設定	 

			}

            // 右側面のセンサーが床が無いと反応した場合
			else{

				*turn = NEXT_TURN_LEFT;           					// 旋回関数に左旋回から旋回をさせるように設定 

			}

		}

        // 角にぶつかったのが初めてではない（ボードの端から端まで移動しきった）時
		 else{

			nRet = CORNER_EXIST;    					// 最後の角に到達したことをメイン文へ返す

		}

		firstCorner++;                            // 旋回した（角にぶつかった）ことを保存(奇数回目でbackhomeになるようにしている)  	
	}

	Serial.println("Out CornerCheck.");
	return nRet;	
}

/**
 * 床方向調整_FloorDirectionTurning						
 * ホワイトボードの側面からはみ出ていないかを確認し，修正する		
 * 床判定の後に呼ばれていることが前提
 */	
void WBRControl::FloorDirectionTurning(){
	
	Serial.println("In FloorDirectionTurning.");

	// (床があるときにセンサが返す値は０[LOW]です)
	int right	= digitalRead(pinFrontRightSensor);	// 前方右センサの状態を取得
	int left	= digitalRead(pinFrontLeftSensor);	// 前方左センサの状態を取得

	// 前方右センサに反応あり（右前に床あり）
	if(right == FLOOR_EXIST){

		// while文の条件内でセンサの値を常に取得し直す。
		while(digitalRead(pinFrontRightSensor) != digitalRead(pinFrontLeftSensor)){

			FloorTurningLeft();						// 左のセンサのはみ出しを修正
			

		}
		
	}

	// 前方左センサに反応あり（左前に床あり）
	else if(left == FLOOR_EXIST){
		
		while(digitalRead(pinFrontRightSensor) != digitalRead(pinFrontLeftSensor)){

			FloorTurningRight();						// 右のセンサのはみ出しを修正		

		}

	}

	// モーターの停止
	analogWrite(pinLeftMotorPWMGo,0);
	analogWrite(pinLeftMotorPWMBack,0);	

	analogWrite(pinRightMotorPWMGo,0);
	analogWrite(pinRightMotorPWMBack,0);

	Serial.println("Out FloorDirectionTurning.");
}

/**
 * FloorTurningRight
 * 床方向調整右回転
 */
void WBRControl::FloorTurningRight(){

	Serial.println("In FloorTurningRight.");

	analogWrite(pinLeftMotorPWMGo,TURN_PWM);
	analogWrite(pinLeftMotorPWMBack,0);			//　PWM値は仮

	analogWrite(pinRightMotorPWMGo,0);
	analogWrite(pinRightMotorPWMBack,0);

	Serial.println("Out FloorTurningRight.");
		
}

/**
 * FloorTurningLeft
 * 床方向調整用左回転
 */
void WBRControl::FloorTurningLeft(){

	Serial.println("In FloorTurningLeft.");

	analogWrite(pinLeftMotorPWMGo,0);
	analogWrite(pinLeftMotorPWMBack,0);	

	analogWrite(pinRightMotorPWMGo,TURN_PWM);
	analogWrite(pinRightMotorPWMBack,0);
	
	Serial.println("Out FloorTurningLeft.");

}

/**
 * GoForward
 * 前進用の関数
 */
void WBRControl::GoForward(){

	Serial.println("In GoForward.");

	analogWrite(pinLeftMotorPWMGo,LEFT_PWM_SPEED);		//左モーターの速度を設定
	analogWrite(pinLeftMotorPWMBack,0);					
	
	analogWrite(pinRightMotorPWMGo, RIGHT_PWM_SPEED);	//右のモータの速度を設定。
	analogWrite(pinRightMotorPWMBack, 0);				

	Serial.println("Out GoForward.");

}

/**
 * 起動前電池残量チェック_StartupBatteryCheck
 * 起動前にバッテリーが十分充電できているか確認する
 * バッテリー容量によって出力電圧が変化していること前提
 */
void WBRControl::StartupBatteryCheck(){
	
	Serial.println("In StartupBatteryCheck.");

	if(analogRead(pinReadBattery) <= START_BATTERY){

		delay(CHARGE_DELAY_TIME); 							//待つことによって充電	

	}
	Serial.println("Out StartupBatteryCheck.");

}

/**
 * 回転_Turn180deg						
 * 180度その場で回転する。内部処理的にはTurn90deg(1回目)→GoForward(車体の横幅一つ分)→Turn90deg(2回目)。
 * goとbackは同時に数値を入れない
 * turn関数は角判定、帰還で指定されている
 */
void WBRControl::Turn180deg(int *turn){

	Serial.println("In Turn180deg.");

		// 回転の際にボードのフチに干渉しないように少し後方へ
		analogWrite(pinLeftMotorPWMGo,0);
		analogWrite(pinLeftMotorPWMBack,LEFT_PWM_SPEED);					
	
		analogWrite(pinRightMotorPWMGo, 0);
		analogWrite(pinRightMotorPWMBack, RIGHT_PWM_SPEED);
		
		Rotary_Encoder(3, 3);

		analogWrite(pinLeftMotorPWMGo, 0);
		analogWrite(pinRightMotorPWMGo, 0);

		// その場で90度回転
		Turn90deg(turn);

		//車体分前進
		analogWrite(pinLeftMotorPWMGo,LEFT_PWM_SPEED);
		analogWrite(pinLeftMotorPWMBack,0);					
	
		analogWrite(pinRightMotorPWMGo, RIGHT_PWM_SPEED);
		analogWrite(pinRightMotorPWMBack, 0);
		
		Rotary_Encoder(1, 1);

		analogWrite(pinLeftMotorPWMGo, 0);
		analogWrite(pinRightMotorPWMGo, 0);

		// もう一度その場で90度回転
		Turn90deg(turn);	

	//　1だったら0、0だったら1のturnを返す
	if(*turn == NEXT_TURN_LEFT){

		*turn = NEXT_TURN_RIGHT;		

	}		

	else{

		*turn = NEXT_TURN_LEFT;		

	}
	Serial.println("Out Turn180deg.");
}

/**
 * 旋回_Turn90deg						
 * 90度その場で回転する		
 * turn関数は角判定、帰還で指定されている
 */
void WBRControl::Turn90deg(int *turn){
	
	Serial.println("In Turn90deg.");

	//右折
	if (*turn == 0) {
				
			analogWrite(pinLeftMotorPWMGo,TURN_PWM);	
			analogWrite(pinLeftMotorPWMBack,0);	//PWM値は仮	
			
			analogWrite(pinRightMotorPWMGo,0);
			analogWrite(pinRightMotorPWMBack,TURN_PWM);		
			
			Rotary_Encoder(2, 2);	//タイヤの回転数を確認			
	}

	// 左折
	else {

			analogWrite(pinLeftMotorPWMGo,0);
			analogWrite(pinLeftMotorPWMBack,TURN_PWM);	
			
			analogWrite(pinRightMotorPWMGo,TURN_PWM);
			analogWrite(pinRightMotorPWMBack,0);

			Rotary_Encoder(2, 2);	//タイヤの回転数を確認
			
	}
	Serial.println("Out Turn90deg.");
}

/**
 * 前方センサーの数が2つか3つか分からないので一応3つセンサーがある前提で書いてあります（´・ω・`）
 * 帰還 BackHome
 * WBRルンバが清掃後、給電所に戻るときに利用される関数
 * 床判定で床がないと判断され角判定で2回目の角にぶつかったと判断されてから帰還するのが大前提
 */
void WBRControl::BackHome(int *turn){
	
	Serial.println("In BackHome.");

	// 磁力センサが実装されていないときは下の文を用いる
	//int magnet_sencer   = digitalRead(pinMagnetSensor);		// 磁力センサの状態を取得(0で未検出、1で検出)
	int magnet_sencer   = 1;
	
	// 磁力センサが給電所からの磁力を検知していない場合(検知するまでループ)

    while(magnet_sencer == 0){ 

		*turn=NEXT_TURN_RIGHT;								//turn関数を固定
    	
		// ホワイトボード右側の端にぶつかるまで前進
		while( FloorCheck()==FLOOR_EXIST){		

			GoForward();	

		}
    	
		Turn90deg(turn);           						// 旋回関数を用いて「その場」で時計回りに90度回転
    	
		delay(MGNET_SERACH_DELAY_TIME);						//　磁力感知がすぐできない場合を考慮  

	}
	Serial.println("Out BackHome.");
}

/**
 *回転数確認_Rotary_Encoder
 *左右のタイヤについているロータリーエンコーダのパルスを見て、タイヤが指定された回数回転するまで監視する
 *回転量の制御には「“x分の1”回転」のxを引数に入力してください。ex.半回転→引数は2(=2分の1回転)
 *センサが羽を通過しているときは出力が0[LOW]、通過していないときは1[HI]が帰ってきます。
 *車輪のサイズは17/6/16時点では56mm,車体の円の直径は180mmです.
 */
void WBRControl::Rotary_Encoder(int RightSpinCount_TargetValue, int LeftSpinCount_TargetValue){

	int RightSpinCount = 0;					//右ロータリーエンコーダのパルスの数を蓄積
	int LeftSpinCount = 0;					//左ロータリーエンコーダのパルスの数を蓄積


	Serial.println("Start Rotary_Encoder Check.");

	//一回転に必要なパルス数にタイヤの回転数をかけて、必要なパルス数を用意する
	//(ここで、呼びされたときに得た引数を用いて必要な回転量分のパルス数を計算)
	RightSpinCount_TargetValue = SPINCOUNT_TARGETVALUE / RightSpinCount_TargetValue;
	LeftSpinCount_TargetValue = SPINCOUNT_TARGETVALUE / LeftSpinCount_TargetValue;	
	
	// Whileループ内で前回のループのセンサの状態を保存する変数を定義
	int beforeR_right = 0;
	int beforeR_left = 0;

	// ロータリーエンコーダーが、関数外で指定されたタイヤの回転数（入力ピンに入ってきたパルスの数）になるまで監視
	while(RightSpinCount_TargetValue >= RightSpinCount && LeftSpinCount_TargetValue >= LeftSpinCount){
	
		int R_right = digitalRead(pinRightRotary_Encoder);	//R_rightに右ロータリーエンコーダの出力を逐一格納
		int R_left = digitalRead(pinLeftRotary_Encoder);	//R_leftに左ロータリーエンコーダの出力を逐一格納
	
		if(R_right==0){		//右ロータリーエンコーダからパルスが出力されたらカウントを追加
			if(beforeR_right == 1){

				RightSpinCount++;
			}
		}
	
		if(R_left==0){		//左ロータリーエンコーダからパルスが出力されたらカウントを追加

			if(beforeR_left == 1){

				LeftSpinCount++;
			}
		}

		// 今回のセンサの状態を次のループへ持ち越す
		beforeR_right = R_right;
		beforeR_left = R_left;

	}

	Serial.println("End Rotary_Encoder Check.");
	
}