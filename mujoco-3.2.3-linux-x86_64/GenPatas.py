import os

with open('link.xml', 'r') as PataGeneral:  
    lectura = PataGeneral.read() # Lee todo el contenido del archivo
    def reemplazo(nombre):
            
        Original = lectura
        
        Original = Original.replace('Pata', nombre)
        Original = Original.replace('colision', 'colision_' + nombre)
        Original = Original.replace('link', nombre + '_link')
        # Original = Original.replace('floor', 'floor_' + nombre)
        # Original = Original.replace('laser', 'laser_' + nombre + '_')
        # Original = Original.replace('name="link', 'name="link_' + nombre + '_')
        Original = Original.replace('Q', nombre + '_Q')
        # Original = Original.replace('suctionCupLink', 'suctionCupLink_' + nombre)
        Original = Original.replace('sensor', 'sensor_' + nombre)
        Original = Original.replace('prx', nombre + '_prx')
        Original = Original.replace('contact', 'contact_' + nombre)
        # Original = Original.replace('tcpLink', 'tcpLink_' + nombre + '_')
        # Original = Original.replace('Base', 'Base_' + nombre)
        # Original = Original.replace('baseSuccionador', 'baseSuccionador_' + nombre)
        
        return Original
    
    with open('ODIN.xml', 'w') as Odin:
        Odin.write(reemplazo("ODIN"))
        
    with open('THOR.xml', 'w') as Thor:
        Thor.write(reemplazo("THOR"))
    
    with open('LOKI.xml', 'w') as Loki:
        Loki.write(reemplazo("LOKI"))
    
    with open('FRIGG.xml', 'w') as Frig:
        Frig.write(reemplazo("FRIGG"))
    