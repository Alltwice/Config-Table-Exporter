using System;
using System.Collections.Generic;
using UnityEngine;

[Serializable]
public class ValidSkillConfig
{
    // 技能ID
    public int id;

    // 技能名
    public string name;

    // 伤害
    public int damage;

    // 冷却时间
    public float cooldown;

    // 是否范围技能
    public bool isAOE;

    // 奖励ID列表
    public List<int> rewardIds;

    // 出生点
    public Vector3 spawnPoint;

    // 路径点
    public List<Vector2> pathPoints;

}

[Serializable]
public class ValidSkillConfigList
{
    public List<ValidSkillConfig> items;
}
