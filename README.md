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

## 4. ESTRUCTURA DE DATOS ESCOGIDA: SUFFIX TREE
La estructura de datos utilizada es un Suffix Tree, también conocido como árbol de sufijos.

En esta estructura, cada nodo representa un carácter. Al recorrer los nodos desde la raíz, se forman palabras o partes de palabras. En el proyecto, se insertan las palabras obtenidas del título y la sinopsis de cada película, y tambien sus sufijos. La idea es insertar los sufijos es que, dada dos cadenas A y B:
    A es una subcadena de B si y solo si A es un prefijo de un sufijo de B.
Basicamente, insertar sufijos al arbol nos permite encontrar todas las cadenas que contienen una subcadena (subcadena que el usuario quiere usar para una busqueda).

Cada palabra (y sus sufijos) queda asociada los IDs de las películas de las que proviene.
  
Por ejemplo, si se tiene la palabra “desembarcar”, se insertan partes como:

desembarcar
esembarcar
sembarcar
embarcar
mbarcar
barcar
arcar
rcar
car
ar
r

Así, si el usuario busca “bar”, el programa puede encontrar películas que contengan palabras como “desembarcar”.

Se escogió el Suffix Tree porque permite realizar búsquedas rápidas por palabra: O(m) donde m es el tamaño de la cadena a buscar.

Tambien estamos usando unordered_map's para hacer busqueda de cadenas mas concretas, como son el el genero de una pelicula o el año de salida. Utilizamos este contenedor ya su busqueda es O(1) la mayoria de las veces.

## 5. MIEMBROS DE LOS NODOS DEL SUFFIX TREE

Cada nodo contiene:
1) Un char que representa la letra de ese nodo.
2) Un contenedor unordered_map de los hijos de el nodo actual. Se utilizo unordered_map ya que la busqueda es O(1) la mayoria de los casos, lo cual nos permite navegar el arbol de manera rapida y consistente.
3) Un vector de IDs de peliculas asociadas a la palabra formada por el camino de un nodo despues de la raiz hasta el nodo actual. Si el vector es vacio sabemos que el nodo actual no es una hoja.
4) Un vector de enteros llamado pesos que sirve para decidir cual pelicula encaja mas con la cadena dada por el usuario en busquedas. De momento, si la palabra aparece en el titulo tiene un peso de 10, mientras que palabras en el plot valen 5 puntos.

Los indices de pesos corresponden con los indices del vector de IDs, es decir, si el ID de una pelicula esta en el indice 5, el peso de la cadena asociada a esa pelicula tambien esta en el indice 5 de pesos.

## 6. ALGORITMO DE INSERCIÓN

El algoritmo de insercion funciona asi:
1) Se va recorriendo los nodos del arbol (desde la raiz), siguiendo el patron de la cadena dada por el usuario hasta que se agote la cadena.

    a) Si existen hijos del nodo actual que siguen el patron, seguir recorriendo el arbol.

    b) Si el nodo actual no tiene hijos que sigan el patron de la cadena
        I. Crear nodo hijo al nodo actual con el siguiente caracter de la cadena.

2) Una vez se acabe la cadena:

    a) Si el vector de IDs del nodo actual no contiene el ID de la pelicula asociada a la cadena, insertamos el ID de la pelicula al vector de IDs. Tambien insertamos el peso de la cadena al vector de pesos.

## 7. ALGORITMO DE BÚSQUEDA
1) Proporcionada una cadena, recorremos los nodos del arbol (desde la raiz), siguiendo el patron de la cadena dada por el usuario hasta que se agote la cadena.

        a) Si existen hijos del nodo actual que siguen el patron, seguir recorriendo el arbol.

        b) Si el nodo actual no tiene hijos que sigan el patron de la cadena
        I. La palabra no esta en el arbol y, por lo tanto, no hay pelicula asociada a la palabra. Detener el algoritmo y devolver un vector de tuplas (ID, peso) vacio.

2) Si la cadena se agoto:

        a) Si el nodo actual no tiene hijos, devolver el vector de IDs y el vector pesos del nodo actual como un vector de tuplas (ID, peso).

        b) Si el nodo actual tiene hijos, crear un vector de tuplas (ID, peso):

            I. Recorrer el subarbol usando Depth-First Search, insertando los valores respectivos del nodo actual al vector de tuplas. Si el ID ya existe en el vector de tuplas, simplemente sumar los pesos.

            II. Una vez recorrido el subarbol, devolver al usuario el vector de tuplas.

## 8. PAGINACION DE RESULTADOS
Los resultados se muestran de 5 en 5. El usuario puede navegar con los siguientes comandos:

| Comando | Accion                                        |
|---------|-----------------------------------------------|
| n       | Siguiente pagina (siguientes 5 resultados)    |
| p       | Pagina anterior                               |
| s       | Seleccionar una pelicula por su indice        |
| q       | Volver a nueva busqueda                       |


## 9. LIKE Y VER MAS TARDE
- Like: Se guarda el ID de la pelicula en un set<int>
- Ver mas tarde: Se guarda en otro set<int>
- Persistencia: Durante la sesion (al cerrar el programa se pierden)
- Al iniciar el programa, se muestran automaticamente las peliculas guardadas en "Ver mas tarde"


## 10. BUSQUEDA POR TAGS
El programa permite buscar por:
- Genero (campo genre)
- Director (campo director)

Actualmente la busqueda principal es por palabra/frase en titulo y sinopsis. La busqueda por tags se realiza comparando directamente con los campos normalizados.


## 11. INTERFAZ DEL USUARIO
La interfaz es por consola con el siguiente flujo:


--- WATCH LATER ---

No hay peliculas guardadas en Ver mas tarde.

Buscar: 

Ejemplo de busqueda:
Buscar: ghost ship

--- RESULTADOS ---
0. ghost ship
1. ghost rider
2. the ghost writer

(n) siguiente | (p) anterior | (s) seleccionar | (q) nueva busqueda: s
Indice: 0

--- PELICULA SELECCIONADA ---
Titulo: ghost ship
Director: steve beck
Genero: horror
Sinopsis: a mysterious ghost ship appears in the ocean...

(l) like | (w) watch later | (b) volver: l
Pelicula marcada con Like.
