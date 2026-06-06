using System.Collections.Generic;
using UnityEngine;

public class ConfigManager
{
    private const string ConfigRoot = "Configs/";
    private static ConfigManager _instance;
    private bool _initialized;

    public static ConfigManager Instance
    {
        get
        {
            if (_instance == null)
            {
                _instance = new ConfigManager();
            }
            return _instance;
        }
    }

    private readonly Dictionary<int, ValidSkillConfig> validSkillConfigDict = new Dictionary<int, ValidSkillConfig>();

    private ConfigManager()
    {
        LoadAll();
    }

    public void LoadAll()
    {
        if (_initialized)
        {
            return;
        }

        LoadValidSkillConfig();
        _initialized = true;
    }

    private void LoadValidSkillConfig()
    {
        TextAsset textAsset = Resources.Load<TextAsset>(ConfigRoot + "ValidSkillConfig");
        if (textAsset == null)
        {
            Debug.LogError("[ConfigManager] JSON 文件不存在: " + ConfigRoot + "ValidSkillConfig");
            return;
        }

        ValidSkillConfigList list = JsonUtility.FromJson<ValidSkillConfigList>(textAsset.text);
        if (list == null || list.items == null)
        {
            Debug.LogError("[ConfigManager] JSON 解析失败: ValidSkillConfig");
            return;
        }

        validSkillConfigDict.Clear();
        foreach (ValidSkillConfig item in list.items)
        {
            if (item == null)
            {
                continue;
            }
            if (validSkillConfigDict.ContainsKey(item.id))
            {
                Debug.LogError($"[ConfigManager] ValidSkillConfig 存在重复 id: {item.id}");
                continue;
            }
            validSkillConfigDict.Add(item.id, item);
        }
    }

    public ValidSkillConfig GetValidSkillConfig(int id)
    {
        LoadAll();
        if (validSkillConfigDict.TryGetValue(id, out ValidSkillConfig config))
        {
            return config;
        }
        Debug.LogError($"[ConfigManager] ValidSkillConfig 未找到 id: {id}");
        return null;
    }

}
