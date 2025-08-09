import sys
import re

# Este script da formato a un archivo .txt con líneas de diferentes estructuras.
# Se recomienda ejecutarlo desde la terminal pasando los nombres de archivo de entrada y salida como argumentos.

# ...existing code...

def agrupar_por_nombre(archivo_formateado, archivo_agrupado):
    nombres = ["FRIGG", "TYR", "LOKI", "FREYJA"]
    grupos = {nombre: [] for nombre in nombres}
    nombre_actual = None

    with open(archivo_formateado, 'r') as fin:
        for linea in fin:
            for nombre in nombres:
                if linea.startswith(nombre + "\t"):
                    nombre_actual = nombre
                    grupos[nombre].append(linea)
                    break
            else:
                if nombre_actual:
                    grupos[nombre_actual].append(linea)

    with open(archivo_agrupado, 'w') as fout:
        for nombre in nombres:
            fout.writelines(grupos[nombre])

def formatear_linea(linea):
    # Si se trata del campo NOMBRE se elimina los : y los saltos de linea. Se utiliza \t como separador
    if linea.endswith(":\n"):
        nombre = linea.strip(":\n")
        return f"{nombre}\t"
    # Si se trata del campo Q, se elimina el identificador. Se utiliza \t como separador
    elif linea.startswith("Q"):
        linea = linea.removeprefix("Q").strip(" \n")
        linea = linea.replace(";", "\t")
        return f"{linea}"
    # Si se trata del campo Torque, se elimina el identificador. Se utiliza \t como separador
    elif linea.startswith("Torque"):
        linea = linea.removeprefix("Torque").strip(" \n")
        linea = linea.replace(";", "\t")
        return f"{linea}"
    elif linea.startswith("MotorD"):
        linea = linea.removeprefix("MotorD").strip(" \n")
        linea = linea.replace(";", "\t")
        return f"{linea}"
    # Ejemplo: Si la línea empieza con una fecha en formato YYYY-MM-DD
    elif re.match(r'^\s*-?\d+\.?\d*;-?\d+\.?\d*;-?\d+\.?\d*;\s*$', linea):
        linea = linea.strip(" \n")
        linea = linea.replace(";", "\t")
        return f"{linea}"
    elif re.match(r'^\s*-?\d+\s*$', linea):
        numero = linea.strip(" \n")
        return f"{numero}\n"
    # Si no cumple ninguna condición, aplica un formato por defecto
    else:
        return ""

def main():
    # Verifica que se pasen los argumentos necesarios
    if len(sys.argv) != 3:
        print("Uso: python format.py archivo_entrada.txt archivo_salida.txt")
        sys.exit(1)

    archivo_entrada = sys.argv[1]
    archivo_salida = sys.argv[2]

    # Abre el archivo de entrada para leer y el de salida para escribir
    with open(archivo_entrada, 'r') as fin, open(archivo_salida, 'w') as fout:
        for linea in fin:
            # Procesa cada línea usando la función de formateo
            linea_formateada = formatear_linea(linea)
            fout.write(linea_formateada)
    
    
    agrupar_por_nombre(archivo_salida, archivo_salida)

if __name__ == "__main__":
    main()
