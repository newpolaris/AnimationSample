#ifndef _H_APPLICATION_
#define _H_APPLICATION_

struct nk_context;

class Application {
private:
    Application(const Application&);
    Application& operator=(const Application&);
public:
    inline Application() {}
    inline virtual void Initialize() {}
    inline virtual void Update(float inDeltaTime) {}
    inline virtual void Render(float inAspectRatio) {}
    inline virtual void ImGui(nk_context* inContext) {}
    inline virtual void Shutdown() {}
};

#endif