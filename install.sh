#!/bin/bash

echo "Preparando la instalación del software ROMERIN..."

if [ -n "$SUDO_USER" ]; then
    ruta_Escritorio="/home/$SUDO_USER/Escritorio"
else
    echo "Por favor, ejecute este script con sudo."
    exit 1
fi

#Actualizar el sistema
echo -e "\e[36m---------update-----------\e[0m"
apt-get update
apt-get upgrade 

#instalacion de librerias para la simulacion 
echo -e "\e[36m---------Simulation-----------\e[0m"
apt install libglfw3 libglfw3-dev

#cmake
apt install cmake


# Intalacion de Qt
echo -e "\e[36m---------Qt & Qt Creator-----------\e[0m"
apt-get install qtbase5-dev qtcreator


apt install libqt5serialport5 libqt5serialport5-dev
apt install libqt5gamepad5 libqt5gamepad5-dev
apt install clang

# Verificacion de instalacion correcta
desktop_file=$(ls /usr/share/applications/ | grep qtcreator)

if [ -z "$desktop_file" ]; then
    echo "Qt no se instaló correectamente."
else
    # Copiar al escritorio
    cp "/usr/share/applications/$desktop_file" "$ruta_Escritorio/"
    chown "$SUDO_USER:$SUDO_USER" "$ruta_Escritorio/$desktop_file"
    echo $'\n Qt Creator instalado correctamente.\n'
    echo $'\n Recuerde que debe permitir su lanzamiento a través del menú desplegable.\n'
fi



#VS code
echo -e "\e[36m---------VS Code-----------\e[0m"
snap install code --classic

echo -e "\e[36m---------Instalación finalizada-----------\e[0m"