# Análisis de Redes - TP2

## Introducción

El objetivo principal de este trabajo práctico es llevar a cabo una configuración de red utilizando la topología Dumbbell. Esta configuración implica la presencia de tres emisores, tres receptores y dos nodos intermedios. Los emisores se conectan a través de los nodos intermedios para enviar datos a los destinos finales. El propósito del análisis es comprender el comportamiento de los protocolos UDP y TCP, así como las interacciones entre ellos.

## Herramientas Utilizadas

Para lograr los objetivos del trabajo práctico, se emplearon las siguientes herramientas:

- **NS3:** Simulador de redes basado en eventos discretos, utilizado para simular el comportamiento de los nodos de red y analizar el rendimiento de los protocolos de comunicación.

- **NetAnim:** Herramienta de visualización y animación utilizada junto con NS3 para representar gráficamente las simulaciones de redes.

- **Wireshark:** Herramienta de análisis de redes de código abierto que permite capturar y analizar el tráfico de red en tiempo real o a partir de archivos de captura previamente almacenados.

- **TcpTrace:** Herramienta open-source para analizar archivos de capturas de redes del tipo .pcap.

- **Gnuplot:** Programa de trazado de gráficos en dos y tres dimensiones utilizado para generar gráficas y visualizar datos de manera interactiva.

## Descripción de la Red Generada

La simulación de la red se basó en la topología Dumbbell, con 3 emisores, 3 receptores y 2 nodos intermedios. Se siguió una configuración donde los emisores utilizan tanto TCP como UDP, y se aplicaron conexiones cableadas. La descripción del código y la configuración detallada se abordan en el informe.

```
Topologia dumbbell y las Ips de los nodos

n0 -----+       +----- n5  |  10.0.2.1 ---------+          +--------- 10.0.3.2
        |       |          |                    |          |          
n1 -----n3 --- n4----- n6  |  10.0.4.1 -----10.0.1.1--- 10.0.1.2----- 10.0.5.2
        |       |          |                    |          |  
n2 -----+       +----- n7  |  10.0.6.2 ---------+          +--------- 10.0.7.2  
```

## Pruebas Realizadas

Se llevaron a cabo dos escenarios de prueba: uno solo con emisores TCP y otro con emisores TCP y UDP simultáneamente. Cada prueba incluyó un análisis detallado de la red.

### Primer Escenario (TCP)

En este escenario, se analizó el comportamiento de la red con dos emisores TCP. Se evaluó la velocidad de transferencia, el tamaño de la cola de recepción y se realizaron análisis detallados de los paquetes TCP.

### Segundo Escenario (TCP-UDP)

En este caso, se incluyó un nodo emisor UDP junto con los emisores TCP para observar cómo afecta la simultaneidad de ambos protocolos a la red. Se comparó el ancho de banda utilizado por cada emisor y se analizaron los paquetes, incluyendo la pérdida y duplicación de los mismos.

## Conclusiones y Problemas Encontrados

Se presentaron las conclusiones derivadas de las pruebas realizadas, así como los problemas identificados durante el desarrollo del trabajo práctico.

## Contacto

Para consultas o comentarios, no dudes en contactarme:
- [LinkedIn del Equipo](https://www.linkedin.com/in/matias-morales-44409a215/)
- [Correo Electrónico del Equipo](mailto:matias.agustin.morales@gmail.com)

Para mas informacion podes mirar el informe subido al repositorio.
¡Gracias por explorar nuestro trabajo de Análisis de Redes - TP2! 🌐🔍



 


