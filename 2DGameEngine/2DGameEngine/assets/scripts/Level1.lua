local map_texture_asset_id

local current_system_hour = os.date("*t").hour
local is_day_time = current_system_hour >= 9 and current_system_hour <= 18

if is_day_time then
    map_texture_asset_id = "jungle-day"
else 
    map_texture_asset_id = "jungle-night"
end

Level =
{
    -- list of assets
	assets = 
    {   
        -- by default lua starts indices at 1. This makes it so that the table is 0 indiced
        [0] =
        { type = "texture", id = "jungle-day", file ="./assets/tilemaps/jungle.png"}, 
        { type = "texture", id = "jungle-night", file ="./assets/tilemaps/jungle-night.png"}, 
        { type = "texture", id = "chopper-image", file = "./assets/images/chopper-spritesheet.png"}, 
        { type = "texture", id = "tank-image", file = "./assets/images/tank-panther-right.png"}, 
        { type = "texture", id = "truck-image", file = "./assets/images/truck-ford-right.png"}, 
        { type = "texture", id = "radar-image", file = "./assets/images/radar.png"}, 
        { type = "texture", id = "bullet-image", file = "./assets/images/bullet.png"}, 
        { type = "texture", id = "tree-image", file = "./assets/images/tree.png"}, 
        { type = "font", id = "pico-font", file = "./assets/fonts/pico8.ttf", font_size = 8}, 
        { type = "font", id = "pico-font-10", file = "./assets/fonts/pico8.ttf", font_size = 10},
        { type = "font", id = "charriot-font", file = "./assets/fonts/charriot.ttf", font_size= 20}	
        },
    -- map configuration variables
    tilemap = 
    {
        map_file = "./assets/tilemaps/jungle.map",
        texture_asset_id = map_texture_asset_id,
        num_rows = 20,
        num_cols = 25,
        tile_size = 32,
        tile_scale = 2.0
    },
    -- list of the level entities
    entities = 
    {
        [0] =
        {
            -- player
            tag = "player",
            components = 
            {
                transform = 
                {
                    position = {x = 240, y = 100},
                    scale = { x = 1, y = 1},
                    rotatition = 0 -- deg
                },
                rigidbody = 
                {
                    velocity = { x = 0, y = 0 }
                },
                sprite = 
                {
                    texture_asset_id = "chopper-image",
                    width = 32,
                    height = 32,
                    z_index = 5
                },
                animation = 
                {
                    num_frames = 2,
                    speed_rate = 10 -- fps
                },
                box_collider = 
                {
                    width = 32,
                    height = 25,
                    offset = { x = 0, y = 5 }
                },
                health = 
                {
                    health_percentage = 100
                },
                projectile_emitter = 
                {
                    projectile_velocity = { x = 400, y = 400 },
                    projectile_duration = 10000, -- ms
                    repeat_frequency = 0, -- ms
                    hit_percentage_damage = 10,
                    should_collide_with_player = false,
                },
                keyboard_controller = 
                {
                    up_velocity = { x = 0, y = -150 },
                    right_velocity = { x = 150, y = 0 },
                    down_velocity = { x = 0, y = 150 },
                    left_velocity = { x = -150, y = 0 },
                },
                camera_follow = 
                {
                    follow = true
                }
            }   
        },
        {
            -- tank
            group = "enemies",
            components = 
            {
                transform = 
                {
                    position = { x = 200, y = 497 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- deg
                },
                sprite = 
                {
                    texture_asset_id = "tank-image",
                    width = 32,
                    height = 32,
                    z_index = 2
                },
                box_collider = 
                {
                    width = 25,
                    height = 18,
                    offset = { x = 0, y = 7 }
                },
                health = 
                {
                    health_percentage = 100
                },
                projectile_emitter = 
                {
                    projectile_velocity = { x = 100, y = 0 },
                    projectile_duration = 2, -- ms
                    repeat_frequency = 1, -- ms
                    hit_percentage_damage = 20,
                    should_collide_with_player = true
                }
            }
        }
    }
}