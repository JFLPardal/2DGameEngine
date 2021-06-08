local colors = 
		{
			dark_grey = {r = 60, g = 60, b = 60, a = 255},
			light_grey = { r = 200, g = 200, b = 200, a = 255 },
			medium_blue = { r = 0,   g = 100, b = 100, a = 255 }
		}

Level = 
{
	assets = 
	{
		[0] =
		{ type="texture", id="player-character", file="./assets/RoomClearer/dummy.png" },
		{ type="texture", id="bullet-image", file="./assets/RoomClearer/arrow.png"}
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
					position = { x = 300, y = 340 },
					scale = { x = .5, y = .5 },
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
					num_frames = 1,
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
					projectile_velocity = { x = 500, y = 500 },
					repeat_frequency = 0.3,
					should_collide_with_player = false,
					min_velocity_magnitude = 800,
					max_velocity_magnitude = 1600,
					time_to_reach_max_velocity_in_secs = .7,
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
					position = {x = 500, y = 100 }
				},
				sprite =
				{
					texture_asset_id = "player-character",
					width = 64,
					height = 64,
					z_index = 3
				},
				box_collider = { },
				rigidbody = 
				{ 
					time_to_stop_in_secs = 1
				},
				dummy_character = { }
			}
		},
		{
			-- dummy
			group = "enemies",
			components =
			{
				transform = 
				{
					position = {x = 300, y = 370 }
				},
				sprite =
				{
					texture_asset_id = "player-character",
					width = 64,
					height = 64,
					z_index = 3
				},
				box_collider = { },
				rigidbody = 
				{ 
					time_to_stop_in_secs = 2
				},
				dummy_character = { }
			}
		},
		{
			-- dummy
			group = "enemies",
			components =
			{
				transform = 
				{
					position = {x = 600, y = 440 }
				},
				sprite =
				{
					texture_asset_id = "player-character",
					width = 64,
					height = 64,
					z_index = 3
				},
				box_collider = { },
				rigidbody = 
				{ 
					time_to_stop_in_secs = 4
				},
				dummy_character = { }
			}
		},
		{
			-- east wall
			group="obstacles",
			components = 
			{
				transform = { position = {x = 1010, y = 0} },
				box_collider = { width = 10, height = 900}
			}
		},
		{
			-- south wall
			group="obstacles",
			components = 
			{
				transform = { position = {x = 0, y = 750} },
				box_collider = { width = 1100, height = 10}
			}
		},
		{
			-- west wall
			group="obstacles",
			components = 
			{
				transform = { position = {x = 0, y = 0} },
				box_collider = { width = 10, height = 900}
			}
		},
		{
			-- north wall
			group="obstacles",
			components = 
			{
				transform = { position = {x = 0, y = 0} },
				box_collider = { width = 1100, height = 10}
			}
		}
	}
}

level_setup =
{
	background_color = colors.medium_blue
}