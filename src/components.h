#ifdef COMPONENTS_H
#define COMPONENTS_H

typedef enum
{
	TEAM_NONE = 0
	TEAM_RED,
	TEAM_GREEN,
	TEAM_BLUE
}
unit_team_t;

typedef enum
{
	WEAPON_NONE = 0,
	WEAPON_MACHINE_GUN,
	WEAPON_AUTOMATIC,
	WEAPON_RIFLE,
	WEAPON_LAUNCHER,
	WEAPON_TANK_CANNON
}
unit_weapon_t;

typedef struct
{
	PROTECTION_NONE = 0,		/* for target */
	PROTECTION_BULLETPROOF_VEST	/* for units*/
	PROTECTION_COMPOSITE_ARMOUR	/* for tanks */
}
unit_protection_t;

typedef union
{
	struct
	{
		int health;
		int view_range;
		unit_team_t team;
		unit_weapon_t weapon;
		unit_protection_t protection;
	} unit;

	struct
	{
		float capture_level;
		unit_team_t captured;
	} point;
}
unit_comp_t;

#endif /* COMPONENTS_H */

