# Proyecto_progra_III

## Integrantes
* Victor Paolo Soto Ruiz
* Giancarlo Davila Segovia
* Tommy Bryan Loo Leon
* Ignacio German Alvarez 
* Sebastian Alonso Gallegos Montejos


## 1. PREPROCESAMIENTO DE LOS DATOS
El preprocesamiento realiza los siguientes pasos:

1. Leer el archivo CSV.
2. Saltar la primera línea, que contiene los encabezados.
3. Separar correctamente los campos del CSV, respetando comas dentro de comillas.
4. Obtener las columnas necesarias:
   - Release Year
   - Title
   - Origin/Ethnicity
   - Director
   - Cast
   - Genre
   - Wiki Page
   - Plot
5. Convertir el texto a minúsculas para todos los campos menos el año de salida.
6. Eliminar caracteres especiales tal como la '.', '-', ':'.
7. Mantener solo letras, números y espacios.
8. Separar el texto en palabras individuales (tokenizar).
9. Dejar las palabras listas para ser insertadas en el arbol.

## 2. PALABRAS LISTAS PARA SER INGRESADAS A LA ESTRUCTURA

Después del preprocesamiento, el programa obtiene palabras limpias que se pueden insertar en el Trie.

Por ejemplo, si una película tiene esta información:

Título: Ghost Ship
Sinopsis: A mysterious ship appears in the ocean.
Género: Horror
Director: Steve Beck

Las palabras listas para el Trie de titulo-sinopsis serian:

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

    Obtener año y verificar que es un entero
    Obtener título
    Obtener origen
    Obtener elenco
    Obtener pagina de Wikipedia
    Obtener director
    Obtener género
    Obtener sinopsis

    Guardar la película en el vector de películas

    Preparar datos para contruir los Suffix Trees y Mapas necesarios
    Normalizar y tokenizar título
    Normalizar y tokenizar origen
    Normalizar y tokenizar elenco
    Normalizar y tokenizar pagina de Wikipedia
    Normalizar y tokenizar director
    Normalizar y tokenizar género
    Normalizar y tokenizar sinopsis

    Usar tokens de titulo y sinopsis para contruir un Suffix Tree
    Usar tokens de director para contruir un Suffix Tree
    Usar tokens de elenco para contruir un Suffix Tree
    Usar tokens de genero para contruir un Mapa No Ordenado
    Usar tokens de origen para contruir un Mapa No Ordenado
    Usar año para contruir un Mapa No Ordenado

    Para cada palabra obtenida:
        Insertar todos los sufijos de la palabra en un trie o insertar la palabra en un mapa
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


## 10. BUSQUEDA POR TAGS
El programa permite buscar por:
- Genero (campo genre)
- Director (campo director)

Actualmente la busqueda principal es por palabra/frase en titulo y sinopsis. La busqueda por tags se realiza comparando directamente con los campos normalizados.


## 11. INTERFAZ DEL USUARIO
La interfaz es por consola con el siguiente flujo:

Inicio del programa:

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
