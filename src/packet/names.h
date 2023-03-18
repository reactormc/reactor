#ifndef PACKET_NAMES_H
#define PACKET_NAMES_H 1

/* Handshaking State {{{1 */
#define PACKET_HANDSHAKING_IN_HANDSHAKE 0x00
/* }}}1 */

/* Status State {{{1*/
/* Clientbound {{{2 */
#define PACKET_STATUS_OUT_STATUS_RESPONSE 0x00
#define PACKET_STATUS_OUT_PING_RESPONSE 0x01
/* }}}2 */
/* Serverbound {{{2 */
#define PACKET_STATUS_IN_STATUS_REQUEST 0x00
#define PACKET_STATUS_IN_PING_REQUEST 0x01
/* }}}2 */
/* }}}1 */

/* Login State {{{1 */
/* Clientbound {{{2 */
#define PACKET_LOGIN_OUT_DISCONNECT 0x00
#define PACKET_LOGIN_OUT_ENCRYPTION_REQUEST 0x01
#define PACKET_LOGIN_OUT_LOGIN_SUCCESS 0x02
#define PACKET_LOGIN_OUT_SET_COMPRESSION 0x03
#define PACKET_LOGIN_OUT_LOGIN_PLUGIN_REQUEST 0x04
/* }}}2 */
/* Serverbound {{{2 */
#define PACKET_LOGIN_IN_LOGIN_START 0x00
#define PACKET_LOGIN_IN_ENCRYPTION_RESPONSE 0x01
#define PACKET_LOGIN_IN_LOGIN_PLUGIN_RESPONSE 0x02
/* }}}2 */
/* }}}1 */

/* Play State {{{1 */
/* Clientbound {{{2 */
#define PACKET_PLAY_OUT_SPAWN_ENTITY 0x00
#define PACKET_PLAY_OUT_SPAWN_EXPERIENCE_ORB 0x01
#define PACKET_PLAY_OUT_SPAWN_PLAYER 0x02
#define PACKET_PLAY_OUT_ENTITY_ANIMATION 0x03
#define PACKET_PLAY_OUT_AWARD_STATISTICS 0x04
#define PACKET_PLAY_OUT_ACKNOWLEDGE_BLOCK_CHANGE 0x05
#define PACKET_PLAY_OUT_SET_BLOCK_DESTROY_STAGE 0x06
#define PACKET_PLAY_OUT_BLOCK_ENTITY_DATA 0x07
#define PACKET_PLAY_OUT_BLOCK_ACTION 0x08
#define PACKET_PLAY_OUT_BLOCK_UPDATE 0x09
#define PACKET_PLAY_OUT_BOSS_BAR 0x0A
#define PACKET_PLAY_OUT_CHANGE_DIFFICULTY 0x0B
#define PACKET_PLAY_OUT_CLEAR_TITLES 0x0C
#define PACKET_PLAY_OUT_COMMAND_SUGGESTIONS_RESPONSE 0x0D
#define PACKET_PLAY_OUT_COMMANDS 0x0E
#define PACKET_PLAY_OUT_CLOSE_CONTAINER 0x0F
#define PACKET_PLAY_OUT_SET_CONTAINER_CONTENT 0x10
#define PACKET_PLAY_OUT_SET_CONTAINER_PROPERTY 0x11
#define PACKET_PLAY_OUT_SET_CONTAINER_SLOT 0x12
#define PACKET_PLAY_OUT_SET_COOLDOWN 0x13
#define PACKET_PLAY_OUT_CHAT_SUGGESTIONS 0x14
#define PACKET_PLAY_OUT_PLUGIN_MESSAGE 0x15
#define PACKET_PLAY_OUT_DELETE_MESSAGE 0x16
#define PACKET_PLAY_OUT_DISCONNECT 0x17
#define PACKET_PLAY_OUT_DISGUISED_CHAT_MESSAGE 0x18
#define PACKET_PLAY_OUT_ENTITY_EVENT 0x19
#define PACKET_PLAY_OUT_EXPLOSION 0x1A
#define PACKET_PLAY_OUT_UNLOAD_CHUNK 0x1B
#define PACKET_PLAY_OUT_GAME_EVENT 0x1C
#define PACKET_PLAY_OUT_OPEN_HORSE_SCREEN 0x1D
#define PACKET_PLAY_OUT_INIT_WORLD_BORDER 0x1E
#define PACKET_PLAY_OUT_KEEP_ALIVE 0x1F
#define PACKET_PLAY_OUT_CHUNK_DATA_AND_UPDATE_LIGHT 0x20
#define PACKET_PLAY_OUT_WORLD_EVENT 0x21
#define PACKET_PLAY_OUT_PARTICLE 0x22
#define PACKET_PLAY_OUT_UPDATE_LIGHT 0x23
#define PACKET_PLAY_OUT_LOGIN 0x24
#define PACKET_PLAY_OUT_MAP_DATA 0x25
#define PACKET_PLAY_OUT_MERCHANT_OFFERS 0x26
#define PACKET_PLAY_OUT_UPDATE_ENTITY_POSITION 0x27
#define PACKET_PLAY_OUT_UPDATE_ENTITY_POS_AND_ROT 0x28
#define PACKET_PLAY_OUT_UPDATE_ENTITY_ROTATION 0x29
#define PACKET_PLAY_OUT_MOVE_VEHICLE 0x2A
#define PACKET_PLAY_OUT_OPEN_BOOK 0x2B
#define PACKET_PLAY_OUT_OPEN_SCREEN 0x2C
#define PACKET_PLAY_OUT_OPEN_SIGN_EDITOR 0x2D
#define PACKET_PLAY_OUT_PING 0x2E
#define PACKET_PLAY_OUT_PLACE_GHOST_RECIPE 0x2F
#define PACKET_PLAY_OUT_PLAYER_ABILITIES 0x30
#define PACKET_PLAY_OUT_PLAYER_CHAT_MESSAGE 0x31
#define PACKET_PLAY_OUT_END_COMBAT 0x32
#define PACKET_PLAY_OUT_ENTER_COMBAT 0x33
#define PACKET_PLAY_OUT_COMBAT_DEATH 0x34
#define PACKET_PLAY_OUT_PLAYER_INFO_REMOVE 0x35
#define PACKET_PLAY_OUT_PLAYER_INFO_UPDATE 0x36
#define PACKET_PLAY_OUT_LOOK_AT 0x37
#define PACKET_PLAY_OUT_SYNC_PLAYER_POS 0x38
#define PACKET_PLAY_OUT_UPDATE_RECIPE_BOOK 0x39
#define PACKET_PLAY_OUT_REMOVE_ENTITIES 0x3A
#define PACKET_PLAY_OUT_REMOVE_ENTITY_EFFECT 0x3B
#define PACKET_PLAY_OUT_RESOURCE_PACK 0x3C
#define PACKET_PLAY_OUT_RESPAWN 0x3D
#define PACKET_PLAY_OUT_SET_HEAD_ROTATION 0x3E
#define PACKET_PLAY_OUT_UPDATE_SECTION_BLOCKS 0x3F
#define PACKET_PLAY_OUT_SELECT_ADVANCEMENTS_TAB 0x40
#define PACKET_PLAY_OUT_SERVER_DATA 0x41
#define PACKET_PLAY_OUT_SET_ACTION_BAR_TEXT 0x42
#define PACKET_PLAY_OUT_SET_BORDER_CENTER 0x43
#define PACKET_PLAY_OUT_SET_BORDER_LERP_SIZE 0x44
#define PACKET_PLAY_OUT_SET_BORDER_SIZE 0x45
#define PACKET_PLAY_OUT_SET_BORDER_WARNING_DELAY 0x46
#define PACKET_PLAY_OUT_SET_BORDER_WARNING_DISTANCE 0x47
#define PACKET_PLAY_OUT_SET_CAMERA 0x48
#define PACKET_PLAY_OUT_SET_HELD_ITEM 0x49
#define PACKET_PLAY_OUT_SET_CENTER_CHUNK 0x4A
#define PACKET_PLAY_OUT_SET_RENDER_DISTANCE 0x4B
#define PACKET_PLAY_OUT_SET_DEFAULT_SPAWN_POSITION 0x4C
#define PACKET_PLAY_OUT_DISPLAY_OBJECTIVE 0x4D
#define PACKET_PLAY_OUT_SET_ENTITY_METADATA 0x4E
#define PACKET_PLAY_OUT_LINK_ENTITIES 0x4F
#define PACKET_PLAY_OUT_SET_ENTITY_VELOCITY 0x50
#define PACKET_PLAY_OUT_SET_EQUIPMENT 0x51
#define PACKET_PLAY_OUT_SET_EXPERIENCE 0x52
#define PACKET_PLAY_OUT_SET_HEALTH 0x53
#define PACKET_PLAY_OUT_UPDATE_OBJECTIVES 0x54
#define PACKET_PLAY_OUT_SET_PASSENGERS 0x55
#define PACKET_PLAY_OUT_UPDATE_TEAMS 0x56
#define PACKET_PLAY_OUT_UPDATE_SCORE 0x57
#define PACKET_PLAY_OUT_SET_SIMULATION_DISTANCE 0x58
#define PACKET_PLAY_OUT_SET_SUBTITLE_TEXT 0x59
#define PACKET_PLAY_OUT_UPDATE_TIME 0x5A
#define PACKET_PLAY_OUT_SET_TITLE_TEXT 0x5B
#define PACKET_PLAY_OUT_SET_TITLE_ANIMATION_TIMES 0x5C
#define PACKET_PLAY_OUT_ENTITY_SOUND_EFFECT 0x5D
#define PACKET_PLAY_OUT_SOUND_EFFECT 0x5E
#define PACKET_PLAY_OUT_STOP_SOUND 0x5F
#define PACKET_PLAY_OUT_SYSTEM_CHAT_MESSAGE 0x60
#define PACKET_PLAY_OUT_SET_TAB_LIST_HEADER_AND_FOOTER 0x61
#define PACKET_PLAY_OUT_TAG_QUERY_RESPONSE 0x62
#define PACKET_PLAY_OUT_PICKUP_ITEM 0x63
#define PACKET_PLAY_OUT_TELEPORT_ENTITY 0x64
#define PACKET_PLAY_OUT_UPDATE_ADVANCEMENTS 0x65
#define PACKET_PLAY_OUT_UPDATE_ATTRIBUTES 0x66
#define PACKET_PLAY_OUT_FEATURE_FLAGS 0x67
#define PACKET_PLAY_OUT_ENTITY_EFFECT 0x68
#define PACKET_PLAY_OUT_UPDATE_RECIPES 0x69
#define PACKET_PLAY_OUT_UPDATE_TAGS 0x6A
/* }}}2 */
/* Serverbound {{{2 */
#define PACKET_PLAY_IN_CONFIRM_TELEPORTATION 0x00
#define PACKET_PLAY_IN_QUERY_BLOCK_ENTITY_TAG 0x01
#define PACKET_PLAY_IN_CHANGE_DIFFICULTY 0x02
#define PACKET_PLAY_IN_MESSAGE_ACKNOWLEDGMENT 0x03
#define PACKET_PLAY_IN_CHAT_COMMAND 0x04
#define PACKET_PLAY_IN_CHAT_MESSAGE 0x05
#define PACKET_PLAY_IN_CLIENT_COMMAND 0x06
#define PACKET_PLAY_IN_CLIENT_INFORMATION 0x07
#define PACKET_PLAY_IN_COMMAND_SUGGESTIONS_REQUEST 0x08
#define PACKET_PLAY_IN_CLICK_CONTAINER_BUTTON 0x09
#define PACKET_PLAY_IN_CLICK_CONTAINER 0x0A
#define PACKET_PLAY_IN_CLOSE_CONTAINER 0x0B
#define PACKET_PLAY_IN_PLUGIN_MESSAGE 0x0C
#define PACKET_PLAY_IN_EDIT_BOOK 0x0D
#define PACKET_PLAY_IN_QUERY_ENTITY_TAG 0x0E
#define PACKET_PLAY_IN_INTERACT 0x0F
#define PACKET_PLAY_IN_JIGSAW_GENERATE 0x10
#define PACKET_PLAY_IN_KEEP_ALIVE 0x11
#define PACKET_PLAY_IN_LOCK_DIFFICULTY 0x12
#define PACKET_PLAY_IN_SET_PLAYER_POSITION 0x13
#define PACKET_PLAY_IN_SET_PLAYER_POSITION_AND_ROTATION 0x14
#define PACKET_PLAY_IN_SET_PLAYER_ROTATION 0x15
#define PACKET_PLAY_IN_SET_PLAYER_ON_GROUND 0x16
#define PACKET_PLAY_IN_MOVE_VEHICLE 0x17
#define PACKET_PLAY_IN_PADDLE_BOAT 0x18
#define PACKET_PLAY_IN_PICK_ITEM 0x19
#define PACKET_PLAY_IN_PLACE_RECIPE 0x1A
#define PACKET_PLAY_IN_PLAYER_ABILITIES 0x1B
#define PACKET_PLAY_IN_PLAYER_ACTION 0x1C
#define PACKET_PLAY_IN_PLAYER_COMMAND 0x1D
#define PACKET_PLAY_IN_PLAYER_INPUT 0x1E
#define PACKET_PLAY_IN_PONG_PLAY 0x1F
#define PACKET_PLAY_IN_PLAYER_SESSION 0x20
#define PACKET_PLAY_IN_CHANGE_RECIPE_BOOK_SETTINGS 0x21
#define PACKET_PLAY_IN_SET_SEEN_RECIPE 0x22
#define PACKET_PLAY_IN_RENAME_ITEM 0x23
#define PACKET_PLAY_IN_RESOURCE_PACK 0x24
#define PACKET_PLAY_IN_SEEN_ADVANCEMENTS 0x25
#define PACKET_PLAY_IN_SELECT_TRADE 0x26
#define PACKET_PLAY_IN_SET_BEACON_EFFECT 0x27
#define PACKET_PLAY_IN_SET_HELD_ITEM 0x28
#define PACKET_PLAY_IN_PROGRAM_COMMAND_BLOCK 0x29
#define PACKET_PLAY_IN_PROGRAM_COMMAND_BLOCK_MINECART 0x2A
#define PACKET_PLAY_IN_SET_CREATIVE_MODE_SLOT 0x2B
#define PACKET_PLAY_IN_PROGRAM_JIGSAW_BLOCK 0x2C
#define PACKET_PLAY_IN_PROGRAM_STRUCTURE_BLOCK 0x2D
#define PACKET_PLAY_IN_UPDATE_SIGN 0x2E
#define PACKET_PLAY_IN_SWING_ARM 0x2F
#define PACKET_PLAY_IN_TELEPORT_TO_ENTITY 0x30
#define PACKET_PLAY_IN_USE_ITEM_ON 0x31
#define PACKET_PLAY_IN_USE_ITEM 0x32
/* }}}2 */
/* }}}1 */

#endif