# Sistema de Dispensação de Medicamentos Automatizado

## Problema de Saúde Abordado
Este sistema aborda o desafio enfrentado por idosos e pessoas com Alzheimer em seguir um cronograma de medicamentos. A complexidade do gerenciamento de múltiplos medicamentos e horários específicos pode levar a esquecimentos, 
o que impacta negativamente a eficácia do tratamento. Além disso, a autonomia reduzida pode tornar difícil para esses indivíduos lembrarem-se de tomar seus remédios no momento correto, aumentando o risco de não conformidade com o tratamento médico.

## Visão Geral da Solução Proposta
O Sistema de Dispensação de Medicamentos Automatizado é uma solução inovadora para o problema mencionado. Este dispositivo automatizado de dispensação de medicamentos é projetado para fornecer lembretes precisos e oportunos aos usuários, facilitando o cumprimento de seus cronogramas de medicamentos. A combinação de um display I2C para informações visuais, um servo motor para dispensar os medicamentos, um buzzer para alertas sonoros e LEDs indicativos para status do sistema oferece uma abordagem abrangente para garantir que os usuários recebam seus medicamentos conforme prescrito.

## Recursos

- **Display I2C**: Um display de LCD I2C é usado para mostrar informações sobre o tempo restante para tomar o medicamento.
- **Servo Motor**: Um servo motor é acionado para dispensar o medicamento quando o temporizador chega ao fim.
- **Buzzer**: Um buzzer é usado para alertar o usuário quando é hora de tomar o medicamento.
- **LEDs Indicativos**: LEDs vermelho e verde indicam o status do sistema.
- **Botão Simulado**: Um botão simulado inicia o temporizador para testes manuais.

## Componentes

- ESP32
- Display LCD I2C
- Servo Motor
- Buzzer
- LEDs (Vermelho e Verde)
- Botão Simulado

## Simulação
<p align="center">
  <img src="https://github.com/guidxs/GS-EDGE/assets/126616600/a376b241-ec7f-4ee5-9887-b4aa06b4b122" alt="Dispositivo">
</p>

## Configuração

Certifique-se de ter as bibliotecas necessárias instaladas no Arduino IDE:

```bash
# Instalação da biblioteca do ESP32
pio lib install "Espressif ESP32 Dev Module"

# Instalação da biblioteca do Servo para ESP32
pio lib install "ServoESP32"
```

Conecte os componentes conforme descrito no código. Certifique-se de ajustar os pinos no código, se necessário.

## Como Usar:
1. Carregue o código para o ESP32 usando o Arduino IDE ou a Plataforma IO.
2. Conecte os componentes conforme especificado.
3. Pressione o botão simulado para iniciar o temporizador.
4. O display mostrará o tempo restante para tomar o medicamento.
5. Quando o temporizador atingir 0, o buzzer tocará, o servo será acionado, e o LED verde acenderá.
6. Lembre-se de ajustar os tempos e os ângulos do servo conforme necessário para o seu caso de uso específico.
