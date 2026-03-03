# bpe-tokenizer
Este repositorio contiene un tokenizer realizado en python. El tokenizer en cuestion sera desarrollado por el metodo BPE (Byte Pair Encoding) que es una forma simple de compresión de datos en la que el par más común de bytes consecutivos se reemplaza con un byte que no ocurre dentro de esos datos. Se requiere una tabla de reemplazos para reconstruir los datos originales.

### Un ejemplo de como funciona BPE
Si queremos codificar los datos "aaabdaaabac" pordemos hacer el reemplazo "Z=aa" por ser el par de bytes de mayor frecuencia.

Entoncees los datos pasan a ser "ZabdZabac". Ahora se repite el proceso para el par "Y=ab", para obtener "ZYbZYac".

El proceso se puede continuar hasta el extremo en el que no existan mas pares de bytes que se produzcan mas de una vez.


## Generalidades
El tokenizer consistira en 3 programas. El primero ("byte_converter") tomara el corpus a utilizar y lo convertira a su version en bytes, para que pueda ser usado en BPE. El segundo ("trainer") de los programas sera el que aplique el metodo BPE para la generacion de tokens con el texto codificado por el programa 1. El tercero ("encoder") el cual usando las reglas generadas por el programa 2 sera capaz de codificar texto nuevo que tenga por entrada.


Para entrenar el tokenizer se eligio como corpus el texto "Don Quijote" por Miguel de Cervantes Saavedra (1605 y 1615). Una reconocida obra literaria del habla hispana. Usar dicha obra permitira luego el analisis de codificacion con palabras de habla moderna como "computadora".
El tokenizer podria ser entrenado con otro texto si se desea, dado que el resultado de los tokens depende de la entrada al sistema, el corpus.

## Utilizacion de expresiones regulares
Copiando el metodo visto en el paper de GPT-2 para evitar tener tokens de mas de una palabra o tokens repetidos por cada palabra y un signo de puntuacion, siguiendo el ejemplo que el mismo paper da, es ineficiente tener todos los siguientes tokens
'''
'dog'
'dog?'
'dog!'
'dog.'
'dog,'
'''
Para evitar tener multiples tokens por cada palabra se usan expresiones regulares para forzar a que se eviten cierto tipo de merges o reglas.

El que puede verse aplicado a GPT-2 es el siguiente patron (quitando todos casos de apostrofe y letra que no aplican para el español):

r""" ?\p{L}+| ?\p{N}+| ?[^\s\p{L}\p{N}]+|\s+(?!\S)|\s+"""

Dado que mi bpe final fue realizado en C++ el patron cambia al sigueinte equivalente:

R"( ?[a-zA-ZáéíóúÁÉÍÓÚñÑ]+| ?[0-9]+| ?[^\s\w]+|\s+(?!\S)|\s+)"
