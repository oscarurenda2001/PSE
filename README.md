# PSE

https://tasmota.github.io/docs/VL53Lxx/#tasmota-settings-for-single-sensor

En aquest projecte implementarem un lidar que mesurará la distancia, ens permetrà escollir la magnitud de la mesura (mm,cm,dm,m) i els exriurà per pantalla (i si es pot, dir quina magnitud es)

La magnitud s'indica amb els leds en forma binaria:
Led 0 apagat Led 1 apagat : mm
Led 0 apagat Led 1 ences : cm
Led 0 ences Led 1 apagat : dm
Led 0 ences Led 1 ences : m
