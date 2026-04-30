#pragma once

#include <string>
#include <vector>

struct Setting
{
    enum Type { BOOL, FLOAT, INT, DOUBLE, COMBO };

    std::string name;
    Type type;

    bool   bValue  = false;
    float  fValue  = 0.0f;
    float  fMin    = 0.0f;
    float  fMax    = 1.0f;
    int    iValue  = 0;
    int    iMin    = 0;
    int    iMax    = 100;
    double dValue  = 0.0;
    double dMin    = 0.0;
    double dMax    = 1.0;

    std::vector<std::string> comboOptions;

    static Setting Bool(const std::string& name, bool defaultVal)
    {
        Setting s;
        s.name   = name;
        s.type   = BOOL;
        s.bValue = defaultVal;
        return s;
    }

    static Setting Float(const std::string& name, float defaultVal, float min, float max)
    {
        Setting s;
        s.name   = name;
        s.type   = FLOAT;
        s.fValue = defaultVal;
        s.fMin   = min;
        s.fMax   = max;
        return s;
    }

    static Setting Int(const std::string& name, int defaultVal, int min, int max)
    {
        Setting s;
        s.name   = name;
        s.type   = INT;
        s.iValue = defaultVal;
        s.iMin   = min;
        s.iMax   = max;
        return s;
    }

    static Setting Double(const std::string& name, double defaultVal, double min, double max)
    {
        Setting s;
        s.name   = name;
        s.type   = DOUBLE;
        s.dValue = defaultVal;
        s.dMin   = min;
        s.dMax   = max;
        return s;
    }

    static Setting Combo(const std::string& name, int defaultIdx, const std::vector<std::string>& options)
    {
        Setting s;
        s.name         = name;
        s.type         = COMBO;
        s.iValue       = defaultIdx;
        s.comboOptions = options;
        return s;
    }
};

class Module
{
public:
    std::string name;
    std::string category;
    bool        enabled = false;
    int         keyBind = 0;

    Module(const std::string& name, const std::string& category, int key = 0)
        : name(name), category(category), keyBind(key) {}

    virtual ~Module() = default;

    void toggle()
    {
        enabled = !enabled;
        if (enabled)
            onEnable();
        else
            onDisable();
    }

    bool isEnabled() const { return enabled; }

    void addBool(const std::string& name, bool defaultVal)
    {
        m_settings.push_back(Setting::Bool(name, defaultVal));
    }

    void addFloat(const std::string& name, float defaultVal, float min, float max)
    {
        m_settings.push_back(Setting::Float(name, defaultVal, min, max));
    }

    void addInt(const std::string& name, int defaultVal, int min, int max)
    {
        m_settings.push_back(Setting::Int(name, defaultVal, min, max));
    }

    void addDouble(const std::string& name, double defaultVal, double min, double max)
    {
        m_settings.push_back(Setting::Double(name, defaultVal, min, max));
    }

    void addCombo(const std::string& name, int defaultIdx, const std::vector<std::string>& options)
    {
        m_settings.push_back(Setting::Combo(name, defaultIdx, options));
    }

    Setting* getSetting(const std::string& name)
    {
        for (auto& s : m_settings)
            if (s.name == name) return &s;
        return nullptr;
    }

    bool getBool(const std::string& name)
    {
        auto* s = getSetting(name);
        return s ? s->bValue : false;
    }

    float getFloat(const std::string& name)
    {
        auto* s = getSetting(name);
        return s ? s->fValue : 0.0f;
    }

    int getInt(const std::string& name)
    {
        auto* s = getSetting(name);
        return s ? s->iValue : 0;
    }

    double getDouble(const std::string& name)
    {
        auto* s = getSetting(name);
        return s ? s->dValue : 0.0;
    }

    std::string getCombo(const std::string& name)
    {
        auto* s = getSetting(name);
        if (!s || s->comboOptions.empty()) return "";
        int idx = s->iValue;
        if (idx < 0 || idx >= (int)s->comboOptions.size()) return "";
        return s->comboOptions[idx];
    }

    std::vector<Setting>& getSettings() { return m_settings; }

    virtual void onEnable()  {}
    virtual void onDisable() {}
    virtual void onUpdate()  {}
    virtual void onRender()  {}

protected:
    std::vector<Setting> m_settings;
};
