#ifndef D3DCLASS_H
#define D3DCLASS_H

// Windows Classes.
#include <windows.h>
#include <windowsx.h>

// DirectX Classes
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <stdlib.h>

// Qt Classes
#include <QElapsedTimer>
#include <QImage>
#include <QDebug>
#include <QFile>

using namespace DirectX;

class D3DManager
{
public:
    D3DManager();
    D3DManager(const D3DManager&);
    ~D3DManager();

    bool Initialize();
    void Shutdown();

    // Getting whatever error there was.
    QString getError() const { return error; }
    void getScreenDimensions(unsigned int &W, unsigned int &H) { W = screenWidth; H = screenHeight; }

    // Rendering functions
    void RenderImage();
    bool LoadQImage(const QImage &image);

    static bool getWasKeyPressed()  { return keyWasPressed; }


private:

    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

    static const int VERTEX_COUNT = 4;
    static const int INDEX_COUNT = 4;

    QString error;

    // Window Related Parameters
    HINSTANCE m_hinstance;
    HWND m_hwnd;
    unsigned int screenWidth;
    unsigned int screenHeight;
    std::wstring appName;

    // D3D Related parameters.
    IDXGISwapChain* m_swapChain;

    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11DepthStencilState* m_depthStencilState;
    ID3D11DepthStencilView* m_depthStencilView;

    ID3D11Buffer *m_vertexBuffer;
    ID3D11Buffer *m_indexBuffer;

    ID3D11ShaderResourceView* m_textureView;
    ID3D11Texture2D* m_texture;

    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;

    unsigned char *imageData;

private:
    void InitializeWindow();
    bool InitializeD3D();
    bool InitializeVertexAndIndexBuffers();
    bool InitializeShaders();

    bool GenerateImageData(const QImage &image, int &height, int& width);

    /////////////////////////
    // FUNCTION PROTOTYPES //
    /////////////////////////
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    static bool keyWasPressed;

};


#endif // D3DCLASS_H
