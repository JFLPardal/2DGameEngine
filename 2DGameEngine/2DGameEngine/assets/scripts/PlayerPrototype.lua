local colors = 
		{
			dark_grey = {r = 20, g = 20, b = 20, a = 255},
			light_grey = { r = 200, g = 200, b = 200, a = 255 },
			medium_blue = { r = 0,   g = 100, b = 100, a = 255 }
		}

Level = 
{
	assets = 
	{
		[0] =
		{ type="texture", id="player-character", file="./assets/RoomClearer/boxes.png" },
		{ type="texture", id="bullet-image", file="./assets/RoomClearer/bullet.png"}
	},
	entities = 
	{
		[0] =
		{
			tag = "player",
			components =
			{
				transform =
				{ 
					position = { x = 30, y = 30 },
					scale = { x = 1, y = 1 },
					rotation = 0
				},
				sprite = 
				{
					texture_asset_id = "player-character",
					width = 64,
					height = 64,
					z_index = 3
				},
				animation = 
				{
					num_frames = 2,
					speed_rate = 1
				},
				rigidbody = 
				{
					velocity = {x = 0, y = 0 }
				},
				keyboard_controller =
				{
					up_velocity = { x = 0, y = -200 },
                    right_velocity = { x = 200, y = 0 },
                    down_velocity = { x = 0, y = 150 },
                    left_velocity = { x = -150, y = 0 },
				},
				box_collider = {} ,
				projectile_emitter =
				{
					projectile_velocity = { x = 100, y = 100 },
					should_collide_with_player = false
				}
			}
		},
		{
			-- dummy
			group = "enemies",
			components =
			{
				transform = 
				{
					position = {x = 400, y = 100 }
				},
				sprite =
				{
					texture_asset_id = "player-character",
					width = 64,
					height = 64,
					z_index = 3
				},
				box_collider = { },
				dummy_character = { }
			}
		}
	}
}

level_setup =
{
	background_color = colors.medium_blue
}