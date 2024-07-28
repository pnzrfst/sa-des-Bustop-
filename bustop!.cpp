#include <Adafruit_LiquidCrystal.h>
#include <LiquidCrystal.h>

Adafruit_LiquidCrystal lcd(0);

//variavel que guarda a formula usada em sensores ultrass.
const float referenciaSensor = 0.01723;

//variavel que guarda se tem alguem na frente de um dos sensores
bool temAlguemEntrada = false;
bool temAlguemSaida = false;



// variaveis lotacao
int lotacaoMaxima = 30;
int lotacaoAtual = 0;
double esteveLotado;
double esteveVazio;

// variaveis para verificar inatividade
unsigned long tempoMaximoInatividade = 6000;
unsigned long ultimoTempoPassageiro = 0;
bool passouAlguemRecente = false;



//Led e Botão
const int led_naoFimDoDia = 5;
const int botaoFimDia = 4;

bool fimDoDia = false;

//botoes menu
#define botaoCima 11
#define botaoBaixo 12
#define botaoSelecionar 13

bool lerBotaoCima = 1; 
bool leituraAnteriorBotaoCima = 1;

bool lerBotaoBaixo = 1; 
bool leituraAnteriorBotaoBaixo = 1;

bool lerBotaoSelecionar = 1; 
bool leituraAnteriorBotaoSelecionar = 1;

int menuAtual = 1;


/*(echo)recebe o sinal de volta,
assim que bate em um obj. / entrada

(trigger)manda o sinal / saída*/ 
//sensor entrada
const int echoPinEntrada = 6;
const  int triggerPinEntrada = 7;
//sensor saída
const int echoPinSaida = 9;
const int triggerPinSaida = 10;


//variaveis relatorioGeral 
bool relatorioExecutado = false;
int totalPassageiros = 0; // no dia
double ganhosTotais = 0.00;
double perdasTotais = 0;



void setup(){
  //iniciando o serial.
  Serial.begin(9600);
  //--------------------------------
  //iniciando o botao e o led
  pinMode(botaoFimDia, INPUT_PULLUP);
  pinMode(led_naoFimDoDia, OUTPUT);
  //--------------------------------
  //iniciando os botoes do menu.
  pinMode(botaoCima, INPUT_PULLUP);
  pinMode(botaoBaixo, INPUT_PULLUP);
  pinMode(botaoSelecionar, INPUT_PULLUP);
  //iniciando o sensor da entrada
  pinMode(triggerPinEntrada, OUTPUT);
  pinMode(echoPinEntrada, INPUT);
  //------------------------------- 
  //iniciando o sensor da saida
  pinMode(triggerPinSaida, OUTPUT);
  pinMode(echoPinSaida, INPUT);
  //------------------------------
  //ligando o led, ate que seja desligado
  digitalWrite(led_naoFimDoDia, HIGH);
  
  lcd.begin(16,2);
  lcd.setCursor(2,0);
  lcd.print("Bem vindo!");

}

void loop(){
  //le estado dos botoes
  lerBotaoCima = digitalRead(botaoCima);
  lerBotaoBaixo = digitalRead(botaoBaixo);
  lerBotaoSelecionar = digitalRead(botaoSelecionar);
  //
  //atualiza e ve se é fimDoDia
  bool fimDoDiaAtualizado = estadoBotao();
  
  if(fimDoDiaAtualizado){
    fimDoDia = !fimDoDia;
  }
    
  if(fimDoDia){
    lcd.print("         ");
    calcularPerdas();
  	controlarMenus();
    atualizarDisplay();
    if(!relatorioExecutado){
    	relatorioGeral();
     	relatorioExecutado = true;
    }
    digitalWrite(led_naoFimDoDia, LOW);
  }else{
  	sensorEntrada();
  	sensorSaida();
    relatorioExecutado = false;
 	verificarInatividade();
    passouAlguemRecente = false;
  }
  
}

  
//------------------------------
void sensorEntrada(){
    digitalWrite(triggerPinEntrada, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPinEntrada, LOW);

    //mede o tempo
    long duracao = pulseIn(echoPinEntrada, HIGH);
    double distancia = referenciaSensor * duracao; 

    if(distancia <= 336 && temAlguemEntrada == false) {
      temAlguemEntrada = true;
    }else if (distancia > 336 && temAlguemEntrada == true) {
        temAlguemEntrada = false;
      	if(lotacaoAtual < lotacaoMaxima) {
          		ultimoTempoPassageiro = millis();
          		passouAlguemRecente = true;
                uteisRelatorio();
          		Serial.println(ganhosTotais);
          		Serial.println(totalPassageiros);
                lcd.setCursor(0, 1);
          		lcd.clear();
          		lcd.setCursor(0, 1);
                lcd.print("Lot. atual: " + String(lotacaoAtual));
    	}else if(lotacaoAtual >= 30){
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("Lotado...");
          esteveLotado++;
          Serial.print(esteveLotado);
          delay(100);
      	}
   }
}

//----------------------

void sensorSaida(){
  	digitalWrite(triggerPinSaida, HIGH);
    delayMicroseconds(100);
    digitalWrite(triggerPinSaida, LOW);

    //medir o tempo
    long duracao = pulseIn(echoPinSaida, HIGH);
    double distancia = referenciaSensor * duracao;

     if (distancia <= 336 && temAlguemSaida == false) {
        temAlguemSaida = true;
     }else if (distancia > 336 && temAlguemSaida == true) {
        temAlguemSaida = false;
           if(lotacaoAtual > 0){
             ultimoTempoPassageiro = millis();
             passouAlguemRecente = true;
             lotacaoAtual--;
             lcd.clear();
             lcd.setCursor(0, 1);
             lcd.print("Lot. atual: " + String(lotacaoAtual));
           }
     }
} 

//-------------------------

void verificarInatividade(){
  
 unsigned long tempoAtual = millis(); 
 unsigned long tempoDecorrido = tempoAtual - ultimoTempoPassageiro;
  
    if(tempoDecorrido > tempoMaximoInatividade && !passouAlguemRecente){
      esteveVazio++;
      Serial.println(esteveVazio);
    }
} 


//-------------------------

void uteisRelatorio(){
	totalPassageiros++;
    lotacaoAtual++;
    ganhosTotais += 4.40;
}

//----------------------------

void atualizarDisplay() {
  switch (menuAtual) {
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("   << MENU >>   ");
      lcd.setCursor(0, 1);
      lcd.print(">T.Usuarios ");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("   << MENU >>   ");
      lcd.setCursor(0, 1);
      lcd.print(">Ganhos  ");
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("   << MENU >>   ");
      lcd.setCursor(0, 1);
      lcd.print(">Perdas ");
      break;
    case 10:
      lcd.setCursor(0, 0);
      lcd.print("T.Usuarios");
      lcd.setCursor(0, 1);
      lcd.print(totalPassageiros);
      break;
    case 20:
      lcd.setCursor(0, 0);
      lcd.print("T.Ganhos");
      lcd.setCursor(0, 1);
      lcd.print(ganhosTotais);
      break;
    case 30:
      lcd.setCursor(0, 0);
      lcd.print("T.Perdas");
      lcd.setCursor(0, 1);
      lcd.print(perdasTotais);
      break;
  }
}


//-----------------------------------------------------------------
int calcularPerdas(){
	perdasTotais = ((esteveLotado + esteveVazio) * 4.40);
  	return perdasTotais;
}
  
//-----------------------------------------------------------------
void relatorioGeral(){
  double mediaPassageiros = totalPassageiros / lotacaoMaxima;
  
    if(mediaPassageiros >= lotacaoMaxima){
       Serial.println("ampliar linha");
    }else if(mediaPassageiros == lotacaoMaxima){
      Serial.println("suficiente");
    }else{
      Serial.println("reduzir linha.");
    }
}

//----------------------------------

bool estadoBotao(){
  int estadoBotaoFim = digitalRead(botaoFimDia);

  	if(estadoBotaoFim == HIGH){// o botão é pressionado
      digitalWrite(led_naoFimDoDia, LOW);// apaga o LED
      return true;
    }else { // quando o botão não é pressionado
      digitalWrite(led_naoFimDoDia, HIGH); // acende o LED
      return false;
    }
}

//*---------------------------------------

void controlarMenus(){
    // botao cima
    if (!lerBotaoCima && leituraAnteriorBotaoCima) {
      lcd.clear();
      if (menuAtual == 1) menuAtual = 2;
      else if (menuAtual == 2) menuAtual = 3;
      else if (menuAtual == 3) menuAtual = 1;    
      delay(30); 
    }
    leituraAnteriorBotaoCima = lerBotaoCima;

    // botao baixo
    if (!lerBotaoBaixo && leituraAnteriorBotaoBaixo) {
      lcd.clear();
      if (menuAtual == 1) menuAtual = 3;
      else if (menuAtual == 2) menuAtual = 1;
      else if (menuAtual == 3) menuAtual = 2;  
      delay(30); 
    }
    leituraAnteriorBotaoBaixo = lerBotaoBaixo;

    // botao selecionar
    if (!lerBotaoSelecionar && leituraAnteriorBotaoSelecionar) {
      lcd.clear();
      if (menuAtual == 1) menuAtual = 10;
      else if (menuAtual == 2) menuAtual = 20;
      else if (menuAtual == 3) menuAtual = 30;
      else if (menuAtual == 10) menuAtual = 1;
      else if (menuAtual == 20) menuAtual = 2;
      else if (menuAtual == 30) menuAtual = 3;         
      delay(30); 
    }
    leituraAnteriorBotaoSelecionar = lerBotaoSelecionar;
}
