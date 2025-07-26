#include <windows.h> // API de Windows para manejo de archivos/disco
#include <iostream> // Para entrada/salida estándar (cout, cerr)
#include <iomanip> // Para formato de salida (hex, setw, setfill)
#include <malloc.h> // Para _aligned_malloc y _aligned_free

int main()
{
    // Abrir disco C directamente
    HANDLE manejador_disco = CreateFileW(
        L"\\\\.\\C:", // Ruta al disco C:
        GENERIC_READ | GENERIC_WRITE, // Permisos de lectura y escritura
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Compartir acceso
        nullptr, // Sin seguridad
        OPEN_EXISTING, // Abrir si existe
        FILE_FLAG_NO_BUFFERING, // Sin caché
        nullptr // Sin plantilla de archivo
    );

    // Buffer alineado para 512 bytes (tamaño de sector)
    void* buffer = _aligned_malloc(512, 512);

    // Leer primer sector desde offset 0
    DWORD bytes_leidos;

    // Lee 512 bytes del disco al buffer
    ReadFile(
        manejador_disco, // El identificador del disco/archivo abierto con CreateFile()
        buffer, // Puntero al buffer donde se guardarán los datos leídos
        512, // Número de bytes a leer (debe ser múltiplo del tamaño de sector)
        &bytes_leidos, // Dirección de una variable donde se almacenará el número real de bytes leídos
        nullptr // NULL para operación síncrona (si no, usar estructura OVERLAPPED para I/O asíncrono)
    );

    // Conversión de puntero
    uint8_t* datos = static_cast<uint8_t*>(buffer);

    // Mostrar tabla 16x16 (256 bytes)
    for (int fila = 0; fila < 16; fila++)
    {
        for (int col = 0; col < 16; col++)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)datos[fila * 16 + col] << " ";
        }

        std::cout << "\n";
    }

    // Liberar memoria
    _aligned_free(buffer);
    CloseHandle(manejador_disco);

    // Detener el programa hasta presionar una tecla
    system("pause");

    return 0;
}
