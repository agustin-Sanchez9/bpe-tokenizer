# bpe-tokenizer

Este repositorio contiene un tokenizer realizado en Python. El tokenizer en cuestión es desarrollado mediante el método BPE (Byte Pair Encoding), una forma simple de compresión de datos en la que el par más común de bytes consecutivos se reemplaza con un byte que no ocurre dentro de esos datos. Se requiere una tabla de reemplazos para reconstruir los datos originales.

### Un ejemplo de cómo funciona BPE

Si queremos codificar los datos "aaabdaaabac" podemos hacer el reemplazo "Z=aa" por ser el par de bytes de mayor frecuencia.

Entonces los datos pasan a ser "ZabdZabac". Ahora se repite el proceso para el par "Y=ab", para obtener "ZYbZYac".

El proceso se puede continuar hasta el extremo en el que no existan más pares de bytes que se produzcan más de una vez.

## Generalidades

El tokenizer consiste en 3 programas. El primero (`byte_converter`) toma el corpus a utilizar y lo convierte a su versión en bytes, para que pueda ser usado en BPE. El segundo (`trainer`) aplica el método BPE para la generación de tokens con el texto codificado por el programa 1. El tercero (`encoder`), usando las reglas generadas por el programa 2, es capaz de codificar texto nuevo que reciba como entrada.

Para entrenar el tokenizer se eligió como corpus el texto "Don Quijote de la Mancha" de Miguel de Cervantes Saavedra (1605 y 1615), una reconocida obra literaria del habla hispana. Usar dicha obra permite luego el análisis de codificación con palabras de habla moderna como "computadora".

El tokenizer podría ser entrenado con otro texto si se desea, dado que el resultado de los tokens depende de la entrada al sistema: el corpus.

## Utilización de expresiones regulares

Copiando el método visto en el paper de GPT-2 para evitar tener tokens de más de una palabra o tokens repetidos por cada palabra y un signo de puntuación, es ineficiente tener todos los siguientes tokens:

```
'dog'
'dog?'
'dog!'
'dog.'
'dog,'
```

Para evitar tener múltiples tokens por cada palabra se usan expresiones regulares que fuerzan a evitar cierto tipo de merges o reglas.

El patrón aplicado en GPT-2 es el siguiente (quitando los casos de apóstrofe y letra que no aplican para el español):

```
r""" ?\p{L}+| ?\p{N}+| ?[^\s\p{L}\p{N}]+|\s+(?!\S)|\s+"""
```

Dado que mi BPE final fue realizado en C++, el patrón cambia al siguiente equivalente:

```
R"( ?[a-zA-ZáéíóúÁÉÍÓÚñÑ]+| ?[0-9]+| ?[^\s\w]+|\s+(?!\S)|\s+)"
```

Aplicar estas reglas de expresiones regulares logró optimizar los tokens. Se logró que con 5000 tokens existiera uno que fuese "Quijote", cosa que sin la aplicación de regex no se lograba ni con 10000 tokens.
