# Tarefa U4.C6.1

Esta tarefa envolveu o uso de quase todos os periféricos vistos até
agora: a matriz de LEDs ws2812b, os botões, o LED RGB no centro da
placa, e o display ssd1306.

## Execução

Para rodar este código, é possível utilizar o CMake para a compilação.
Ao iniciar o programa na placa, as seguintes funcionalidades estão
disponíveis:

- Botão A: ligar ou desligar o verde no LED central;
- Botão B: Ligar ou desligar o azul no LED central;
- Caracteres enviados via serial: mudanças nos displays;

Os displays, atualizando a ~20Hz, devem responder com base na entrada:

- A matriz ws2812b irá mostrar o último dígito numérico enviado via
    serial;

- O display ssd1306 irá mostrar o evento mais recente, além do último
    caractere pressionado;

## Créditos

O código para uso do display `ssd1306` foi feito com base no código
disponível no [repositório do prof. Wilton
Lacerda](https://github.com/wiltonlacerda/EmbarcaTechU4C6/).
