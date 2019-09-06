#include "d3dmanager.h"

// Initializing keyWasPressed
bool D3DManager::keyWasPressed = false;

D3DManager::D3DManager()
{
    m_swapChain = nullptr;
    m_device = nullptr;
    m_deviceContext = nullptr;
    m_renderTargetView = nullptr;
    m_depthStencilBuffer = nullptr;
    m_depthStencilState = nullptr;
    m_depthStencilView = nullptr;
    imageData = nullptr;
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_textureView = nullptr;
    m_texture = nullptr;
    m_vertexBuffer = nullptr;
    m_pixelShader = nullptr;
    m_layout = nullptr;
}

D3DManager::D3DManager(const D3DManager& other)
{
    Q_UNUSED(other);
}

D3DManager::~D3DManager()
{
}

bool D3DManager::Initialize()
{

    InitializeWindow();

    if (!InitializeD3D()) return false;

    if (!InitializeVertexAndIndexBuffers()) return false;

    if (!InitializeShaders()) return false;

    return true;

}

///////////////////////////////////////// INTIALIZATION FUNCTIONS //////////////////////////////////////////////

void D3DManager::InitializeWindow(){

    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;

    // Get the instance of this application.
    m_hinstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(nullptr));

    appName = L"D3DWindow";

    // Setup the windows class with default settings.
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = appName.data();
    wc.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    // Determine the resolution of the clients desktop screen.
    screenWidth  = static_cast<unsigned int>(GetSystemMetrics(SM_CXSCREEN));
    screenHeight = static_cast<unsigned int>(GetSystemMetrics(SM_CYSCREEN));

    // Setup the screen settings depending on whether it is running in full screen or in windowed mode.

    // If full screen set the screen to maximum size of the users desktop and 32bit.
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth  = static_cast<unsigned long>(screenWidth);
    dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(screenHeight);
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Change the display settings to full screen.
    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

    // Set the position of the window to the top left corner.
    posX = posY = 0;

    // Create the window with the screen settings and get the handle to it.
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW,
                            appName.data(),
                            appName.data(),
                            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
                            posX, posY, screenWidth, screenHeight, nullptr, nullptr, m_hinstance, nullptr);

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);

    // Hide the mouse cursor.
    // ShowCursor(false);

}

bool D3DManager::InitializeD3D(){

    error = "";

    HRESULT result;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Texture2D* backBufferPtr;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_VIEWPORT viewport;

    // Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // Set to a single back buffer.
    swapChainDesc.BufferCount = 1;

    // Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = static_cast<unsigned int>(screenWidth);
    swapChainDesc.BufferDesc.Height = static_cast<unsigned int>(screenHeight);

    // Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Set the refresh rate of the back buffer. (As fast as possible)
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

    // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Set the handle for the window to render to.
    swapChainDesc.OutputWindow = m_hwnd;

    // Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // Set to full screen
    swapChainDesc.Windowed = false;

    // Set the scan line ordering and scaling to unspecified.
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Discard the back buffer contents after presenting.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // Don't set the advanced flags.
    swapChainDesc.Flags = 0;

    // Set the feature level to DirectX 11.
    featureLevel = D3D_FEATURE_LEVEL_11_0;

    // Create the swap chain, Direct3D device, and Direct3D device context.
    result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1,
                                           D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, nullptr, &m_deviceContext);
    if(FAILED(result)){
        error = "Failed to create D3D Devices and Swap Chain";
        return false;
    }

    // Get the pointer to the back buffer.
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBufferPtr));
    if(FAILED(result)){
        error = "Failed getting pointer to the backbuffer";
        return false;
    }

    // Create the render target view with the back buffer pointer.
    result = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView);
    if(FAILED(result))
    {
        error = "Failed to create a render target wiht the backbuffer pointer";
        return false;
    }

    // Release pointer to the back buffer as we no longer need it.
    backBufferPtr->Release();
    backBufferPtr = nullptr;

    // Initialize the description of the depth buffer.
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = static_cast<unsigned int>(screenWidth);
    depthBufferDesc.Height = static_cast<unsigned int>(screenHeight);
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
    if(FAILED(result))
    {
        error = "Failed to create the 2D texture for the depth buffer";
        return false;
    }

    // Initialize the description of the stencil state.
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = false;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state.
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if(FAILED(result))
    {
        error = "Failed creating the Depth Stencil State";
        return false;
    }

    // Set the depth stencil state.
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

    // Initailze the depth stencil view.
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if(FAILED(result))
    {
        error = "Failed creating the Depth Stencil View";
        return false;
    }

    // Bind the render target view and depth stencil buffer to the output render pipeline.
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    // Setup the viewport for rendering.
    viewport.Width = static_cast<float>(screenWidth);
    viewport.Height = static_cast<float>(screenHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport.
    m_deviceContext->RSSetViewports(1, &viewport);

    return true;

}

bool D3DManager::InitializeVertexAndIndexBuffers(){

    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    error = "";

    // Create the vertex array.
    vertices = new VertexType[static_cast<unsigned int>(VERTEX_COUNT)];
    if(!vertices)
    {
        return false;
    }

    // Create the index array.
    indices = new unsigned long[static_cast<unsigned int>(INDEX_COUNT)];
    if(!indices)
    {
        return false;
    }

    // Load the vertex array with data.
    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
    vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

    vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top left.
    vertices[1].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
    vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

    vertices[3].position = XMFLOAT3(1.0f, 1.0f, 0.0f);  // Top right.
    vertices[3].texture = XMFLOAT2(1.0f, 0.0f);

    // Load the index array with data.
    indices[0] = 0;  // Bottom left.
    indices[1] = 1;  // Top left.
    indices[2] = 2;  // Bottom right.
    indices[3] = 3;  // Top Right

    // Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * static_cast<unsigned int>(VERTEX_COUNT);
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    result = m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result)){
        error = "Failed to create vertex buffer";
        return false;
    }

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * static_cast<unsigned int>(INDEX_COUNT);
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = m_device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Release the arrays now that the vertex and index buffers have been created and loaded.
    delete [] vertices;
    vertices = nullptr;

    delete [] indices;
    indices = nullptr;

    return true;

}

bool D3DManager::InitializeShaders(){

    // Initialize the vertex and pixel shaders.
    QString internal_ps_resource = ":/shaders/pixel_shader.hlsl";
    QString internal_vs_resource = ":/shaders/vertex_shader.hlsl";
    QString local_ps_resource = "local_pixel_shader.hlsl";
    QString local_vs_resource = "local_vertex_shader.hlsl";

    // Saving the files locally.
    QFile::copy(internal_ps_resource,local_ps_resource);
    QFile::copy(internal_vs_resource,local_vs_resource);

    // Name as STD W Strings.
    std::wstring vertexShader = local_vs_resource.toStdWString();
    std::wstring pixelShader =  local_ps_resource.toStdWString();

    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;

    // Initialize the pointers this function will use to null.
    errorMessage = nullptr;
    vertexShaderBuffer = nullptr;
    pixelShaderBuffer = nullptr;

    // Compile the vertex shader code.
    result = D3DCompileFromFile(vertexShader.data(), nullptr, nullptr, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                    &vertexShaderBuffer, &errorMessage);
    if(FAILED(result)){
        // If the shader failed to compile it should have writen something to the error message.
        if(errorMessage){
            error = "Failed to compile the Vertex Shader. Error is: ";
            char* compileErrors = static_cast<char*>(errorMessage->GetBufferPointer());
            error = error + compileErrors;
            return false;
        }
        else error = "Could not load the vertex shader file";
        return false;
    }

    // Compile the pixel shader code.
    result = D3DCompileFromFile(pixelShader.data(), nullptr, nullptr, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                    &pixelShaderBuffer, &errorMessage);
    if(FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if(errorMessage){
            error = "Failed to compile the Pixel Shader. Error is: ";
            char* compileErrors = static_cast<char*>(errorMessage->GetBufferPointer());
            error = error + compileErrors;
            return false;
        }        
        else error = "Could not load the pixel shader file";
        return false;
    }

    // Create the vertex shader from the buffer.
    result = m_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);
    if(FAILED(result)){
        error = "Failed to Create Vertex Shader";
        return false;
    }

    // Create the pixel shader from the buffer.
    result = m_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);
    if(FAILED(result)){
        error = "Failed to Create Pixel Shader";
        return false;
    }

    // Create the vertex input layout description.
    // This setup needs to match the VertexType stucture in the ModelClass and in the shader.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = m_device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
                       vertexShaderBuffer->GetBufferSize(), &m_layout);
    if(FAILED(result))
    {
        return false;
    }

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    return true;


}

///////////////////////////////////////// RENDERING //////////////////////////////////////////////

void D3DManager::RenderImage(){

    float color[4];
    unsigned int stride;
    unsigned int offset;

    ////// Scene set up
    // Setup the color to clear the buffer to.
    color[0] = 0; color[1] = 0; color[2] = 0; color[3] = 0;

    // Clear the back buffer.
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

    // Clear the depth buffer.
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    ////// Model Rendering
    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    m_deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles (strip)
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    ////// Texture Rendering
    // Set shader texture resource in the pixel shader.
    m_deviceContext->PSSetShaderResources(0, 1, &m_textureView);

    // Set the vertex input layout.
    m_deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    m_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

    // Render the triangle.
    m_deviceContext->DrawIndexed(static_cast<unsigned int>(INDEX_COUNT), 0, 0);

    // Present as fast as possible.
    m_swapChain->Present(0, 0);

}

///////////////////////////////////////////// SHUTDOWN ///////////////////////////////////////////////////////

void D3DManager::Shutdown()
{
    // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
    if(m_swapChain){
        m_swapChain->SetFullscreenState(false, nullptr);
    }

    if(m_depthStencilView){
        m_depthStencilView->Release();
        m_depthStencilView = nullptr;
    }

    if(m_depthStencilState){
        m_depthStencilState->Release();
        m_depthStencilState = nullptr;
    }

    if(m_depthStencilBuffer){
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = nullptr;
    }

    if(m_renderTargetView){
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }

    if(m_deviceContext){
        m_deviceContext->Release();
        m_deviceContext = nullptr;
    }

    if(m_device){
        m_device->Release();
        m_device = nullptr;
    }

    if(m_swapChain){
        m_swapChain->Release();
        m_swapChain = nullptr;
    }

    if(m_indexBuffer){
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }

    if(m_vertexBuffer){
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }

    if (m_textureView){
        m_textureView->Release();
        m_textureView = nullptr;
    }

    if (m_texture){
        m_texture->Release();
        m_texture = nullptr;
    }

    if (m_vertexShader){
        m_vertexShader->Release();
        m_vertexShader = nullptr;
    }

    if (m_pixelShader){
        m_pixelShader->Release();
        m_pixelShader = nullptr;
    }

    if (m_layout){
        m_layout->Release();
        m_layout = nullptr;
    }

    // Show the mouse cursor.
    // ShowCursor(true);

    // Fix the display settings if leaving full screen mode.
    ChangeDisplaySettings(nullptr, 0);

    // Remove the window.
    DestroyWindow(m_hwnd);
    m_hwnd = nullptr;

    // Remove the application instance.
    UnregisterClass(appName.data(), m_hinstance);
    m_hinstance = nullptr;

    return;
}

///////////////////////////////////////// LOAD IMAGE TO TEXTURE //////////////////////////////////////////////

bool D3DManager::LoadQImage(const QImage &image){

    int height, width;
    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT hResult;
    unsigned int rowPitch;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

    if (!GenerateImageData(image,height,width)){
        return false;
    }

    if (!imageData){
        qDebug() << "NO IMAGE DATA";
    }

    // Setup the description of the texture.
    textureDesc.Height = static_cast<unsigned int>(height);
    textureDesc.Width = static_cast<unsigned int>(width);
    textureDesc.MipLevels = 0;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    // Create the empty texture.
    hResult = m_device->CreateTexture2D(&textureDesc, nullptr, &m_texture);
    if(FAILED(hResult))
    {
        qDebug() << "Failed creating 2D Texture";
        return false;
    }

    // Set the row pitch of the targa image data.
    rowPitch = (static_cast<unsigned int>(width) * 4) * sizeof(unsigned char);

    // Copy the targa image data into the texture.
    m_deviceContext->UpdateSubresource(m_texture, 0, nullptr, imageData, rowPitch, 0);

    // Setup the shader resource view description.
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view for the texture.
    hResult = m_device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
    if(FAILED(hResult))
    {
        qDebug() << "Failed creating Shader Resource View"  << hResult;
        return false;
    }

    // Generate mipmaps for this texture.
    m_deviceContext->GenerateMips(m_textureView);

    // Release the image data now that the image data has been loaded into the texture.
    delete [] imageData;
    imageData = nullptr;

    return true;

}

bool D3DManager::GenerateImageData(const QImage &image, int &height, int &width){

    int imageSize, index, i, j;
    height = image.height();
    width = image.width();

    if (image.isNull()) return false;

    //qDebug() << "Loaded Image of " << width << "x" << height;

    // Calculate the size of the 32 bit image data.
    imageSize = width * height * 4;

    // Allocate memory for the targa destination data.
    imageData = new unsigned char[static_cast<unsigned int>(imageSize)];
    if(!imageData)
    {
        qDebug() << "Could not create the image data";
        return false;
    }

    // Initialize the index into the targa destination data array.
    index = 0;

    // Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
    for(j=0; j<height; j++)
    {
        for(i=0; i<width; i++)
        {
            QRgb px = image.pixel(i,j);

            imageData[index + 0] = static_cast<unsigned char>(qRed(px));  // Red.
            imageData[index + 1] = static_cast<unsigned char>(qGreen(px));  // Green.
            imageData[index + 2] = static_cast<unsigned char>(qBlue(px));  // Blue
            imageData[index + 3] = static_cast<unsigned char>(qAlpha(px));  // Alpha

            index += 4;
        }

    }

    return true;

}

///////////////////////////////////////// WINDOW CONTROL FUNCTIONS //////////////////////////////////////////////
LRESULT CALLBACK D3DManager::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch(umessage)
    {
    // Check if the window is being destroyed.
    case WM_DESTROY:{
        //PostQuitMessage(0);
        return 0;
    }

    // Check if the window is being closed.
    case WM_CLOSE:{
        //PostQuitMessage(0);
        return 0;
    }

    case WM_KEYDOWN:{
        // If a key is pressed send it to the input object so it can record that state.
        //m_Input->KeyDown(static_cast<unsigned int>(wparam));
        keyWasPressed = true;
        return 0;
    }

    // Check if a key has been released on the keyboard.
    case WM_KEYUP:{
        // If a key is released then send it to the input object so it can unset the state for that key.
        //m_Input->KeyUp(static_cast<unsigned int>(wparam));
        return 0;
    }

        // Any other messages send to the default message handler as our application won't make use of them.
    default:
    {
        return DefWindowProc(hwnd, umessage, wparam, lparam);
    }
    }
}
