# Proyecto_progra_III

## Integrantes
* Victor Paolo Soto Ruiz
* Giancarlo Davila Segovia
* Tommy Bryan Loo Leon
* Ignacio German Alvarez 
* Sebastian Alonso Gallegos Montejos

## Documentacion

# Enunciado del proyecto (BORRAR EN EL ULTIMO COMMIT)

## Plataforma de Streaming
El objetivo del proyecto final es implementar una plataforma de **streaming**. Un programa que administre la **búsqueda y visualización** de la sinopsis de películas. Para ello se debe implementar las siguientes operaciones:

* El programa debe leer la base de datos en forma **.csv**. La base de datos puede ser descargada desde el siguiente [link] (https://drive.google.com/file/d/1UJkRuCF8UD92W_DT7S8dXCYzaR_9wqB_/view?usp=sharing). El grupo es responsable del **pre-procesamiento de los datos**.
* El programa debe cargar el contenido corregido del archivo en un **Árbol** que permita la búsqueda rápida de una película. Los caracteres (letras y números) deben ser los valores que se almacenen en los nodos del Árbol. Puede utilizar como referencia estructuras como los **Tries**, **Suffix Trees**, etc." La elección del tipo de Árbol queda a criterio del grupo y debe ser justificada y documentada en el repositorio.
* Para buscar una película se debe utilizar una **palabra, frase o sub-palabra**. Ejemplo:
  - Si se busca la palabra "barco", el programa debería encontrar todas las películas en las cuales la palabra "barco" este en el título o sinópsis.
  - Si se busca la frase "barco fantasma", el programa debería encontrar todas las películas en las cuales las palabras "barco" y/o "fantasma" este en el título o sinópsis.
  - Si se busca el string "bar", el programa debería encontrar todas las películas en las cuales el string "bar" este en el título o sinópsis (El string "bar" podría ser parte de una palabra).
* También se debe poder buscar películas por un **Tag**: director, casting, generero, etc. 
* Al buscar películas deben de aparecer la cinco más **importantes** y una opción para visualizar las siguientes cinco coincidencias. El grupo **debe implementar un algoritmo** para determinar que pélicula tiene más importancia en una búsqueda.
* Al seleccionar una película, se debe visualizar la sinopsis y las opciones **Like** y **Ver más tarde** .
* Al iniciar el programa la plataforma debería mostrar las películas que fueron añadidas en **Ver más tarde**. Además, se debe visualizar las películas similares a las que el usuario les dio **Like** (implemente su propio algoritmo).

## Requisitos
* Grupos de cinco personas como máximo y de tres como mínimo. No se aceptarán grupos de dos o una persona.
* Subir el programa a un repositorio en Github. **En el repositorio debe de estar toda la documentación sobre el proyecto**.
* La exposición del proyecto es **Presencial**. En la presentación de la semana 8, las exposiciones son con respecto a los avances que hayan conseguido.
* Todo el programa, desde la lectura hasta la búsqueda de palabras, debe estar en C++.
* Cumplir con la rúbrica del proyecto.
* Fecha de presentación: La semana 8 (avance) y la semana 16 (final).


## 1. PREPROCESAMIENTO DE LOS DATOS
El preprocesamiento realiza los siguientes pasos:

1. Leer el archivo CSV.
2. Saltar la primera línea, que contiene los encabezados.
3. Separar correctamente los campos del CSV, respetando comas dentro de comillas.
4. Obtener las columnas necesarias:
   - Title
   - Director
   - Genre
   - Plot
5. Convertir el texto a minúsculas.
6. Eliminar caracteres especiales.
7. Mantener solo letras, números y espacios.
8. Separar el texto en palabras individuales.
9. Dejar las palabras listas para ser insertadas en el Trie.

## 2. PALABRAS LISTAS PARA SER INGRESADAS A LA ESTRUCTURA

Después del preprocesamiento, el programa obtiene palabras limpias que se pueden insertar en el Trie.

Por ejemplo, si una película tiene esta información:

Título: Ghost Ship
Sinopsis: A mysterious ship appears in the ocean.
Género: Horror
Director: Steve Beck

Las palabras listas para la estructura serían:

ghost
ship
a
mysterious
ship
appears
in
the
ocean

Cada palabra se asocia al ID de la película correspondiente.

Ejemplo:

ghost -> película 0
ship -> película 0
mysterious -> película 0
appears -> película 0
ocean -> película 0

Esto permite que, si el usuario busca la palabra “ship”, el programa pueda encontrar rápidamente las películas relacionadas con esa palabra.

## 3. PSEUDOCÓDIGO DE INGRESO DE DATOS A LA ESTRUCTURA
Algoritmo: Cargar películas en el Trie

Abrir archivo CSV

Leer y saltar la primera línea del archivo, que contiene los encabezados

Crear contador de ID en 0

Para cada línea del archivo:
    Separar correctamente los campos del CSV
    Verificar que existan las columnas necesarias

    Crear una nueva película
    Asignar un ID único a la película

    Obtener título
    Obtener director
    Obtener género
    Obtener sinopsis

    Normalizar título
    Normalizar director
    Normalizar género
    Normalizar sinopsis

    Guardar la película en el vector de películas

    Unir título y sinopsis en un solo texto

    Separar el texto en palabras

    Para cada palabra obtenida:
        Guardar la palabra en el índice de coincidencias exactas
        Insertar todos los sufijos de la palabra en el Trie
        Asociar cada inserción con el ID de la película

    Aumentar el ID en 1

Cerrar archivo CSV

## 4. ESTRUCTURA DE DATOS ESCOGIDA: TRIE
La estructura de datos utilizada es un Trie, también conocido como árbol de prefijos.

En esta estructura, cada nodo representa un carácter. Al recorrer los nodos desde la raíz, se forman palabras o partes de palabras. En el proyecto, se insertan las palabras obtenidas del título y la sinopsis de cada película.

Cada palabra queda asociada al ID interno de la película de la que proviene. Además, para permitir búsquedas por sub-palabras, también se insertan los sufijos de cada palabra.
  
Por ejemplo, si se tiene la palabra “desembarcar”, se insertan partes como:

desembarcar
embarcar
barcar
arcar
car

Así, si el usuario busca “bar”, el programa puede encontrar películas que contengan palabras como “desembarcar”.

Se escogió el Trie porque permite realizar búsquedas rápidas por palabra, prefijo o sub-palabra, cumpliendo con el requisito principal del proyecto.

## 5. ALGORITMO DE INSERCIÓN

Para insertar una palabra en el Trie, el programa recorre sus caracteres desde el primero hasta el último.

Si un carácter no existe en el árbol, se crea un nuevo nodo. Luego, se avanza al siguiente carácter hasta completar la palabra.

En cada nodo se guarda el ID de la película relacionada, para que luego el programa pueda recuperar las películas que coinciden con una búsqueda.

Pseudocódigo:

Entrada: palabra, idPelicula

nodoActual = raíz

Para cada carácter de la palabra:
    Si el carácter no existe:
        Crear nuevo nodo

    Avanzar al nodo del carácter

    Guardar idPelicula en ese nodo

Marcar fin de palabra

## 6. ALGORITMO DE BÚSQUEDA
La búsqueda empieza cuando el usuario ingresa una palabra, frase o sub-palabra.

Primero, el programa normaliza la consulta. Luego la divide en palabras. Cada palabra se busca tanto en el índice de coincidencias exactas como en el Trie.

Las películas encontradas reciben un puntaje y luego se ordenan de mayor a menor relevancia.

Pseudocódigo:

Entrada: consulta

Normalizar consulta

Separar consulta en palabras

Para cada palabra:
    Buscar coincidencia exacta
    Buscar coincidencia parcial en el Trie
    Sumar puntaje a las películas encontradas

Ordenar películas por puntaje

Mostrar resultados de cinco en cinco


