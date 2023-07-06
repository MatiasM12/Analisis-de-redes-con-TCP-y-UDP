# Tp2-Sor2

 

## Analisis de redes

El siguiente trabajo fue realizado en la materia sistemas operativos y redes 2. El mismo consiste en realizar una topologia de red dumbbell como se muestra a continuacion y saturar el canal central para poder realizar un analisis detallado de la misma.

// Topologia dumbbell y las Ips de los nodos
n0 -----+       +----- n5  |  10.0.2.1 ---------+          +--------- 10.0.3.2
        |       |          |                    |          |          
n1 -----n3 --- n4----- n6  |  10.0.4.1 -----10.0.1.1--- 10.0.1.2----- 10.0.5.2
        |       |          |                    |          |  
n2 -----+       +----- n7  |  10.0.6.2 ---------+          +--------- 10.0.7.2  

Ademas para los analisis se plantearon dos escenarios, el primero con dos nodos tcp emitiendo a la vez y el segundos con dos nodos tcp y uno udp emitiendo a la vez. Para dicho analisis se utilizaron herramientas como Wireshark y Gnuplot para obtener graficos e informacion de la red.
