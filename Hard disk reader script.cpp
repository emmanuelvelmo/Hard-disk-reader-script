#include <windows.h>
#include <iostream>
#include <iomanip>
#include <malloc.h>

// Clase para manejar la lectura directa del disco
class lector_disco
{
    private:
        HANDLE manejador_disco; // Manejador para acceder al dispositivo físico
        DWORD tam_sector; // Tamaño del sector en bytes (usualmente 512)

    public:
        // Constructor inicializa manejador inválido y sector tamaño por defecto
        lector_disco() : manejador_disco(INVALID_HANDLE_VALUE), tam_sector(512) {}

        // Destructor cierra el manejador si está abierto
        ~lector_disco()
        {
            if (manejador_disco != INVALID_HANDLE_VALUE)
            {
                CloseHandle(manejador_disco); // Cierra el handle abierto
            }
        }

        // Abre el disco en modo lectura sin buffer y con acceso compartido total
        bool abrir(const wchar_t* ruta)
        {
            manejador_disco = CreateFileW(
                ruta, // Ruta al dispositivo (ej. "\\\\.\\D:")
                GENERIC_READ, // Solo lectura
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Compartir acceso
                nullptr, // Sin atributos de seguridad especiales
                OPEN_EXISTING, // Abrir solo si existe
                FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN, // Acceso sin cache, secuencial
                nullptr // Sin plantilla
            );

            if (manejador_disco == INVALID_HANDLE_VALUE) return false; // Si falla abrir, retornar falso

            tam_sector = 512; // Asignar tamaño sector fijo (mejorable)

            return true; // Éxito en apertura
        }

        // Lee datos sin procesar desde offset (en bytes) directamente al buffer
        bool leer_bruto(uint64_t offset, void* buffer, size_t tam)
        {
            LARGE_INTEGER posicion; // Variable para el desplazamiento de lectura
            posicion.QuadPart = offset; // Establece el offset de lectura en bytes

            // Mueve el puntero de archivo al offset especificado
            SetFilePointer(manejador_disco, posicion.LowPart, &posicion.HighPart, FILE_BEGIN);

            DWORD bytes_leidos;
            // Lee tamaño 'tam' bytes desde disco en buffer, devuelve true si leyó exactamente 'tam'
            return ReadFile(manejador_disco, buffer, static_cast<DWORD>(tam), &bytes_leidos, nullptr) && bytes_leidos == tam;
        }

        // Muestra en pantalla un volcado hexadecimal en filas x columnas desde offset
        void mostrar_volcado_hex(uint64_t offset, size_t filas, size_t columnas)
        {
            size_t tam = filas * columnas; // Total bytes a mostrar
            // Calcula tamaño alineado al tamaño del sector para lectura sin buffer
            size_t tam_alineado = ((tam + tam_sector - 1) / tam_sector) * tam_sector;

            // Reserva buffer alineado a sector (requisito para lectura sin buffering)
            void* buffer = _aligned_malloc(tam_alineado, tam_sector);

            // Lee datos del disco desde offset al buffer alineado
            leer_bruto(offset, buffer, tam_alineado);

            uint8_t* datos = static_cast<uint8_t*>(buffer);
            
            // Recorre filas
            for (size_t fila = 0; fila < filas; ++fila)
            {
                // Recorre columnas
                for (size_t col = 0; col < columnas; ++col)
                {
                    // Imprime byte en hex con formato 2 dígitos, rellenando con 0 si necesario
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)datos[fila * columnas + col] << " ";
                }

                std::cout << "\n"; // Salto de línea al terminar fila
            }
            _aligned_free(buffer); // Libera el buffer alineado
        }
};

int main()
{
    lector_disco lector; // Crear instancia del lector de disco
    lector.abrir(L"\\\\.\\C:"); // Abrir disco C (ruta especial de dispositivo)
    lector.mostrar_volcado_hex(0, 16, 16); // Mostrar tabla desde offset 0

    system("pause"); // Pausa para que usuario vea salida

    return 0; // Termina programa exitosamente
}