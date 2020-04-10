#include "Timer.h"

Timer timer;

const int PINO_TERMOMETRO = 0; // o termometro está ligado no A0
const int PINO_AQUECEDOR = 2; // o secador de cabelos está ligado ao D2
const float temperatura_referencia_superior = 73;
const float temperatura_referencia_inferior = 67;

float temperatura = 0;

bool em_aquecimento;

int amostras = 0;
int id_timer_aquecimento;
int id_timer_funcionamento;
int id_timer_desativacao_estado_em_aquecimento;
int id_timer_desligamento_geral;


void setup() {
  Serial.begin(9600);
  // Configura o pino do relé do aquecedor e aciona o relé
  pinMode(PINO_AQUECEDOR, OUTPUT);
  delay(300);
  ligar_aquecedor();
  // Configura o timer para verificação do aquecimento - 240s (O tempo gasto para que a temperatura dentro do involucro atinja ≃ 70°C
  id_timer_aquecimento = timer.after(240000, verificar_aquecimento);
  id_timer_funcionamento = timer.every(1000, funcionamento, 2040);
  id_timer_desligamento_geral = timer.after(2040000, desligar_tudo);
  Serial.println("tempo,temperatura");
}

void verificar_aquecimento() {
  // interrompe funcionamento do aquecedor
  desativar_aquecimento();
  ler_temperatura();
  if (temperatura < temperatura_referencia_inferior) {
    desligar_aquecedor();
    Serial.println("O Aquecedor nao esta aquecendo suficientemente e sera desligado para economia de energia, favor realizar ajustes necessarios");
    timer.stop(id_timer_funcionamento);
    timer.stop(id_timer_desligamento_geral);
  }
  timer.stop(id_timer_aquecimento);
}

void funcionamento() {
  ler_temperatura();
  if (temperatura < temperatura_referencia_inferior && !em_aquecimento) {
    ligar_aquecedor();
    id_timer_desativacao_estado_em_aquecimento = timer.after(120000, desativar_aquecimento);
  }
  else if (temperatura > temperatura_referencia_superior && !em_aquecimento) {
    desligar_aquecedor();
  }
}

void desligar_tudo() {
  desativar_aquecimento();
  desligar_aquecedor();
}

void desativar_aquecimento() {
  em_aquecimento = false;
}

void desligar_aquecedor() {
  digitalWrite(PINO_AQUECEDOR, LOW);
}

void ligar_aquecedor() {
  em_aquecimento = true;
  digitalWrite(PINO_AQUECEDOR, HIGH);
}

void enviar_mensagem_computador() {
  Serial.print(amostras);
  Serial.print(",");
  Serial.println(temperatura);
}

void ler_temperatura() {
  temperatura = (analogRead(PINO_TERMOMETRO) * 0.4887585532);
  amostras ++;
  enviar_mensagem_computador();
}

void loop() {
  timer.update();
}
