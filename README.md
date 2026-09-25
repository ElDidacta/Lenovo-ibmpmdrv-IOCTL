# Lenovo-ibmpmdrv-IOCTL-Poc
[+] Análisis técnico y PoC de comportamiento anómalo en el IOCTL 0x22242C del controlador ibmpmdrv.sys de Lenovo


# Lenovo-ibmpmdrv-IOCTL-Poc

### ⚠️ Descargo de Responsabilidad / Disclaimer
Este proyecto fue creado exclusivamente con fines educativos, de investigación de seguridad y desarrollo de pruebas de concepto (PoC). El autor no se hace responsable del uso indebido o malicioso de la información aquí contenida.

---

## 📝 Descripción General
Este repositorio contiene el análisis técnico y una Prueba de Concepto (PoC) enfocada en la interacción directa con el código de control **IOCTL 0x22242C** expuesto por el controlador oficial de Lenovo **`ibmpmdrv.sys`** (Lenovo Power Management Driver). 

El objetivo de este análisis es evaluar los mecanismos de control de acceso y el comportamiento del controlador cuando recibe datos estructurados desde el espacio de usuario sin privilegios elevados.

---

## 🛠️ Detalles del Entorno de Prueba
* **Controlador Objetivo:** `ibmpmdrv.sys` (Lenovo Power Management)
* **Enlace Simbólico:** `\\.\IBMPmDrv`
* **Código IOCTL:** `0x22242C`
* **Privilegios Requeridos:** **Usuario Estándar (No Administrador / Low Integrity)**

---

## ⚙️ ¿Cómo Funciona la PoC?

El programa realiza las siguientes operaciones técnicas estructuradas en tres etapas principales:

1. **Apertura del Controlador sin Privilegios:**  
   El programa utiliza la función nativa de Windows `CreateFileA` para solicitar un manejador (*handle*) al enlace simbólico del controlador (`\\.\IBMPmDrv`). La prueba demuestra que las Listas de Control de Acceso (ACL) del dispositivo permiten que un **usuario sin privilegios de administrador** obtenga un manejador válido con permisos de lectura y escritura (`GENERIC_READ | GENERIC_WRITE`).

2. **Interacción con el IOCTL:**  
   Una vez obtenido el manejador, se invoca la función `DeviceIoControl` enviando el código específico `0x22242C`. En esta fase se pasan búferes de datos de entrada estructurados (en la PoC representados conceptualmente mediante relleno `0x41414141`) directamente hacia las rutinas del controlador que operan en el espacio de Kernel (*Kernel Mode*).

3. **Mecanismo de Reverse Shell (Espacio de Usuario):**  
   Si la comunicación con el IOCTL responde de manera exitosa confirmando la interacción, el código redirige el flujo en modo usuario para establecer una consola inversa. Utilizando la API `Winsock2` (`WSASocket` y `WSAConnect`), el programa se conecta a una IP externa y un puerto configurados (por ejemplo, un listener en un equipo remoto). Acto seguido, manipula la estructura `STARTUPINFOA` para clonar e igualar los canales estándar de entrada, salida y error (`hStdInput`, `hStdOutput`, `hStdError`) con el descriptor del socket de red, ejecutando finalmente `cmd.exe` de forma oculta a través de `CreateProcessA`.

---

## ⚠️ Estado del Reporte con el Fabricante (Lenovo)

Este hallazgo y su correspondiente Prueba de Concepto fueron reportados formalmente al equipo de seguridad de Lenovo para su evaluación. Lamentablemente, **Lenovo no le dio importancia al reporte ni reconoció el comportamiento anómalo como una vulnerabilidad válida** dentro de su alcance de soporte.

### Razones Técnicas de la Discrepancia:
* **Perspectiva del Autor:** El hecho de que cualquier proceso ejecutado por un usuario común (no administrador) pueda abrir exitosamente un manejador al controlador y enviar datos directamente a funciones de Kernel representa una debilidad de diseño en el endurecimiento (*hardening*) de las ACLs del driver, ampliando la superficie de ataque del sistema operativo.
* **Perspectiva del Fabricante:** Por lo general, los fabricantes u organismos de asignación de vulnerabilidades desestiman estos reportes si el envío de datos al IOCTL no provoca una falla catastrófica inmediata en el espacio de kernel (como una denegación de servicio o Pantallazo Azul - BSOD), o si no se demuestra de manera fehaciente una alteración directa de los *Tokens* de seguridad de Windows para escalar de forma efectiva a privilegios de `NT AUTHORITY\SYSTEM`.

Debido al cierre del reporte sin planes de mitigación por parte del fabricante, se publica este análisis con el fin de aportar al estudio de la seguridad en controladores de Windows (*Driver Security Auditing*).

