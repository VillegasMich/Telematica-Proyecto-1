# Proyecto 1 Chat ~ Cliente / Servidor

<!--toc:start-->

- [Proyecto 1 Chat ~ Cliente / Servidor](#proyecto-1-chat-cliente-servidor)
  - [Desarrolladores](#desarrolladores)
  - [Introducción](#introducción)
  - [Desarrollo](#desarrollo)
    - [Ejecución](#ejecución)
    - [Cliente](#cliente)
    - [Servidor](#servidor)
  - [Aspectos Logrados y Posibles mejoras](#aspectos-logrados-y-posibles-mejoras)
    - [Que logramos](#que-logramos)
    - [Que podemos mejorar](#que-podemos-mejorar)
  - [Conclusiones](#conclusiones)
  - [Referencias](#referencias)
  <!--toc:end-->

## Desarrolladores

- [Julian Mejia](https://github.com/julimejia)
- [Esteban Muriel](https://github.com/estebanm30)
- [Manuel Villegas](https://github.com/VillegasMich)

## Introducción

En el mundo actual es importante mantener una comunicación constante entre personas de manera cómoda, rápida y confiable, sobre todo si dichas personas se encuentran separadas por grandes distancias. Es por esto que este proyecto se basa en la implementación de un mecanismo basado en el modelo de [_cliente/servidor_](https://www.geeksforgeeks.org/client-server-model/) que se encargue de la comunicación de varios usuarios de manera [_concurrente_](<https://en.wikipedia.org/wiki/Concurrency_(computer_science)>).

Dicha aplicación se encarga de la gestión de los mensajes, tanto como para el cliente como para el servidor, y su correcto manejo de acuerdo al protocolo encargado de dicho proceso. El protocolo con el cual se comunica la aplicación es definido como **MJEP** _(Manuel Julián Esteban Protocol)._

## Desarrollo

Esta aplicación maneja tanto el cliente como el servidor en el lenguaje de programación [C](<https://en.wikipedia.org/wiki/C_(programming_language)>), donde encontramos la siguiente estructura:

```
...
|
src/ ----|
         client.c
         |
         server.c
         |
         mjep.h
         |
         mjep.c
         |
         config.h
```

Donde _client.c_ se encuentra el programa del cliente (código que ejecuta quien se quiere conectar al servidor), _server.c_ (código que se mantiene ejecutando el servidor) y _mjep_ _.h_ y _.c_, la definición del protocolo y su implementación respectiva. Por ultimo el archivo _config.h_ contiene una configuracion básica para el servidor.

Toda la respectiva documentación del protocolo se puede encontrar [aquí]().

Además, se utilizan la [API de sockets de la universidad de Berkeley](https://en.wikipedia.org/wiki/Berkeley_sockets) para el manejo de las conexiones entre el servidor y los clientes a un nivel superior a la capa 4 del [modelo TCP/IP](https://www.geeksforgeeks.org/tcp-ip-model/) y otra importante como el manejo de la concurrencia con el uso de la [API de pthreads](https://en.wikipedia.org/wiki/Pthreads) para el manejo de los hilos tanto como en el cliente como en el servidor.

### Ejecución

1. Clonamos el repositorio en la máquina.

`$ git clone https://github.com/VillegasMich/Telematica-Proyecto-1.git`

2. Compilamos la aplicación cliente

`$ make`

3. Ejecutamos el archivo correspondiente.

`$ ./build/server` para el servidor.

`$ ./build/client` para el cliente.

[!NOTE]
Es necesario ubicar la IP publica del servidor en el archivo `src/config.h` antes de compilar.

### Cliente

El cliente lo definimos como la persona quien se quiere conectar con el servidor porque requiere usar los servicios que ese le presta, en este caso el transporte de los mensajes y su redireccionamiento a otro cliente. Este cliente tiene un periodo de vida de ejecución, es decir, el programa solo se va a ejecutar hasta que el mismo cliente esté satisfecho y se realice la desconexión del servidor.

La estructura del cliente es la siguiente:

1. El cliente inicia la aplicación `./client`.
2. Se crea el socket respectivo para conectarse con el servidor (si el servidor está apagado, el programa finaliza avisando que el servidor se encuentra caído).
3. El cliente ingresa su nombre con el que quiere ser registrado en el servidor.
4. El cliente tiene la opción de observar los demás clientes conectados y seleccionar uno para intercambiar mensajes.
5. Cuando dos clientes confirmen su conexión se procede con el intercambio de mensajes.
6. Los clientes intercambian mensaje sin orden ni turnos, hasta que por medio de mensajes reservados se acabe la conexión.
7. Dependiendo de mensaje se acaba el programa o regresa al \#4.

Los mensajes reservados que encontramos dentro de esta aplicación son 2:

- **‘_disconnect_’**: Si el usuario ingresa esta palabra reservada, el programa finaliza su ejecución y se le avisa al otro usuario que debe de salir del chat.

- **_‘exit’_**: Si el usuario utiliza esta palabra reservada, entonces el programa continúa su ejecución, pero finaliza el respectivo chat previamente creado con los dos clientes y se prepara para volver a seleccionar otra persona con quien hablar (\#4 de la estructura anterior).

Finalmente, como se mencionó anteriormente el cliente maneja una estructura concurrente con dos hilos, el principal que se encarga de escuchar los inputs del sistema para después llamar al protocolo, y un segundo hilo para escuchar los mensajes que llegan del servidor (respuestas y/o mensajes del segundo usuario).

### Servidor

Por el otro lado encontramos al servidor, quien definimos como la entidad quien se encarga de prestar un servicio para unos clientes, dicha funcionalidad es el manejo (tanto procesamiento como redireccionamiento) de los mensajes que recibe por parte de un cliente por medio del protocolo **MJEP**. Se espera que el periodo de vida de ejecución sea indefinido en el tiempo, suponiendo que no encontraran complejidades de hardware ni demás condiciones externas.

La estructura del servidor es la siguiente:

1. Inicializa variables como lo son el [pool de hilos](https://en.wikipedia.org/wiki/Thread_pool), su estructura de datos de clientes.
2. Realiza la inicialización del servidor (puerto definido por el protocolo).
3. Comienza a escuchar conexiones de posibles clientes.
4. Cuando un cliente se conecta, se dispone uno de los hilos para manejar ese cliente de manera independiente.
5. Dadas ciertas condiciones, el protocolo finaliza la vida de dicho hilo si el cliente se desconecta.

El servidor también tiene una estructura concurrente (todas las aplicaciones en red lo son, esta no podía ser la excepción), con un hilo principal quien se encarga de comenzar el servidor y escuchar por clientes, cuando un cliente se conecta se selecciona un hilo de la piscina de hilos para manejar ese cliente personalmente. El número de hilos/clientes que soporta el servidor está dado por el mismo protocolo en una variable definida como _BACKLOG_.

## Aspectos Logrados y Posibles mejoras

### Que logramos

- Cumplimos con la definicion, creacion, e implementacion de un protocolo para el intercambio de mensajes por medio de una estructura cliente/servidor.
- Implementamos una aplicacion en red para el intercambio de mensajes entre dos clientes por medio de un servidor utilizando dicho protocolo.
- Dicha aplicacion implementa manejo de concurrencia por medio de distintos hilos para poder realizar diferentes tareas de manera simultanea.
- Definimos y documentamos de manera correcta tanto la aplicación en red como el protocolo.
- Aprendimos a reconocer e implementar conseptos de abstraccion, interfaz, encapsulamiento y des encapsulamiento.
- Implementamos y utilizamos de manera correcta la API de sockets de la universidad de Berkeley.
- Implementamos los conseptos estudiados en el curso de Telematica con el Profesor Juan Carlos Montoya.
- Despliegue del servidor en un servidor en la nube de AWS.

### Que podemos mejorar

- Crear una interfaz de usuario (UI) mas interactiva para la aplicacion.
- Manejo de la informacion desde el lado del servidor, implementar una mejor estructura de datos.
- Escritura de un código aún mas limpio y legible.
- Manejo persistente de la información. Generar un log de información sobre los clientes y mensajes.

## Conclusiones

Con el desarrollo de este trabajo hemos podido mejorar nuestras capacidades como programadores y diseñadores de software, no solo por el especificar e implementar un protocolo creado por nosotros mismos, sino por implementar de manera correcta los conceptos estudiados en clase y durante toda la carrera de Ingeniería de Sistemas.

El uso de un lenguaje de programación como C nos obligó a conocer un nivel mas bajo de la programación (aunque siga siendo alto nivel), y con esto generar un mayor nivel de abstracción
y escritura correcta de código permitiendo un mejor entendimiento de las temáticas vistas en clase.

Finalmente este proyecto nos ha permitido observar como los procesos en red funcionan y como tienen distintas aproximaciones por las cuales se acercan a una misma solución.

## Referencias

- _accept(2) \- Linux manual page_. (s/f). Man7.org. Recuperado el 4 de octubre de 2024, de [https://man7.org/linux/man-pages/man2/accept.2.html](https://man7.org/linux/man-pages/man2/accept.2.html)
- _Beej’s Guide to network programming_. (s/f). Beej.Us. Recuperado el 4 de octubre de 2024, de [https://beej.us/guide/bgnet/](https://beej.us/guide/bgnet/)
- _connect(2) \- Linux manual page_. (s/f). Man7.org. Recuperado el 4 de octubre de 2024, de [https://man7.org/linux/man-pages/man2/connect.2.html](https://man7.org/linux/man-pages/man2/connect.2.html)
- _listen(2) \- Linux manual page_. (s/f). Man7.org. Recuperado el 4 de octubre de 2024, de [https://man7.org/linux/man-pages/man2/listen.2.html](https://man7.org/linux/man-pages/man2/listen.2.html)
- _pthread_create(3) \- Linux manual page_. (s/f). Man7.org. Recuperado el 4 de octubre de 2024, de [https://man7.org/linux/man-pages/man3/pthread_create.3.html](https://man7.org/linux/man-pages/man3/pthread_create.3.html)
- _pthread_detach(3) \- Linux manual page_. (s/f). Man7.org. Recuperado el 4 de octubre de 2024, de [https://man7.org/linux/man-pages/man3/pthread_detach.3.html](https://man7.org/linux/man-pages/man3/pthread_detach.3.html)
- _pthreads(7) \- Linux manual page_. (s/f). Man7.org. Recuperado el 4 de octubre de 2024, de [https://man7.org/linux/man-pages/man7/pthreads.7.html](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- _socket(2) \- Linux manual page_. (s/f). Man7.org. Recuperado el 4 de octubre de 2024, de [https://man7.org/linux/man-pages/man2/socket.2.html](https://man7.org/linux/man-pages/man2/socket.2.html)
- _TCP server-client implementation in C_. (2018, septiembre 12). GeeksforGeeks. [https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/](https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/)
