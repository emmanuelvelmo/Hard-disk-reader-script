#include <windows.h> // API de Windows para manejo de archivos/disco
#include <iostream> // Para entrada/salida estándar (cout, cerr)
#include <iomanip> // Para formato de salida (hex, setw, setfill)
#include <malloc.h> // Para _aligned_malloc y _aligned_free

int main()
{
    // Abrir disco C directamente
    HANDLE manejador_disco = CreateFileW(
        L"\\\\.\\C:", // Ruta al disco C:
        GENERIC_READ, // Solo lectura
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

    ReadFile(manejador_disco, buffer, 512, &bytes_leidos, nullptr);

    // Mostrar tabla 16x16 (256 bytes)
    uint8_t* datos = (uint8_t*)buffer;

    // Tabla 16x16
    for (int fila = 0; fila < 16; fila++)
    {
        for (int col = 0; col < 16; col++)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)datos[fila * 16 + col] << " ";
        }

        std::cout << "\n";
    }

    // Limpieza
    _aligned_free(buffer);
    CloseHandle(manejador_disco);

    system("pause");

    return 0;
}
