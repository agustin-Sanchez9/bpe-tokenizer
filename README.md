# llm-tokenizer
Este repositorio contiene un tokenizer realizado en python. El tokenizer en cuestion sera desarrollado por el metodo BPE (Byte Pair Encoding) que es una forma simple de compresión de datos en la que el par más común de bytes consecutivos se reemplaza con un byte que no ocurre dentro de esos datos. Se requiere una tabla de reemplazos para reconstruir los datos originales.

### Un ejemplo de como funciona BPE
Si queremos codificar los datos "aaabdaaabac" pordemos hacer el reemplazo "Z=aa" por ser el par de bytes de mayor frecuencia.

Entoncees los datos pasan a ser "ZabdZabac". Ahora se repite el proceso para el par "Y=ab", para obtener "ZYbZYac".

El proceso se puede continuar hasta el extremo en el que no existan mas pares de bytes que se produzcan mas de una vez.


El tokenizer consistira en 3 programas. El primero ("conversor") tomara el corpus a utilizar y lo convertira a su version en bytes, para que pueda ser usado en BPE. El segundo ("tokenizer") de los programas sera el que aplique el metodo BPE para la generacion de tokens con el texto codificado por el programa 1. El tercero ("encoder") el cual usando las reglas generadas por el programa 2 sera capaz de codificar texto nuevo que tenga por entrada.


Para entrenar el tokenizer se eligio como corpus el texto "Don Quijote" por Miguel de Cervantes Saavedra (1605 y 1615). Una reconocida obra literaria del habla hispana. Usar dicha obra permitira luego el analisis de como trabajara el tokenizer con palabras de habla moderna como "computadora".
El tokenizer podria ser entrenado con otro texto si se desea, dado que el resultado de los tokens depende de la entrada al sistema, el corpus.
