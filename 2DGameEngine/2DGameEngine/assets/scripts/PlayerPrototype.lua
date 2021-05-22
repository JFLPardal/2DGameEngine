Level = 
{
	assets = 
	{
		[0] =
		{ type="texture", id="player_character", file="./assets/RoomClearer/chopper-spritesheet.png" }
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
					scale = { x = 3, y = 3 },
					rotation = 0
				},
				sprite = 
				{
					texture_asset_id = "player_character",
					width = 32,
					height = 32,
					z_index = 3
				},
				animation = 
				{
					num_frames = 2,
					speed_rate = 20
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
				box_collider = 
				{
					width = 32,
					height = 32
				}
			}
		}
	}
}