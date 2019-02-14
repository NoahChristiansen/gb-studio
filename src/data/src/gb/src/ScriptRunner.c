#include "ScriptRunner.h"
#include "BankData.h"
#include "game.h"

UBYTE script_ptr_bank = 0;
UWORD script_ptr = 0;
UWORD script_start_ptr = 0;
UBYTE script_cmd_args[6] = {0};
UBYTE script_cmd_args_len;

SCRIPT_CMD script_cmds[] = {
    {Script_End_b, 0},           // 0x00
    {Script_Text_b, 2},          // 0x01
    {Script_Goto_b, 2},          // 0x02
    {Script_IfFlag_b, 4},        // 0x03
    {Script_Noop_b, 0},          // 0x04
    {Script_SetFlag_b, 2},       // 0x05
    {Script_ClearFlag_b, 2},     // 0x06
    {Script_ActorSetDir_b, 1},   // 0x07
    {Script_ActorActivate_b, 1}, // 0x08
    {Script_CameraMoveTo_b, 3},  // 0x09
    {Script_CameraLock_b, 1},    // 0x0A
    {Script_Wait_b, 1},          // 0x0B
    {Script_FadeOut_b, 1},       // 0x0C
    {Script_FadeIn_b, 1},        // 0x0D
    {Script_LoadScene_b, 6},     // 0x0E
    {Script_ActorSetPos_b, 2},   // 0x0F
    {Script_ActorMoveTo_b, 2},   // 0x10
    {Script_ShowSprites_b, 0},   // 0x11
    {Script_HideSprites_b, 0},   // 0x12
    {Script_Noop_b, 1},          // 0x13
    {Script_ShowPlayer_b, 0},    // 0x14
    {Script_HidePlayer_b, 0},    // 0x15
    {Script_ActorSetEmote_b, 1}, // 0x16
    {Script_CameraShake_b, 1},   // 0x17
    {Script_ReturnToTitle_b, 0}  // 0x18
};

void ScriptStart(BANK_PTR *events_ptr)
{
  script_ptr_bank = events_ptr->bank;
  script_ptr = ((UWORD)bank_data_ptrs[script_ptr_bank]) + events_ptr->offset;
  script_start_ptr = script_ptr;
}

void ScriptRunnerUpdate()
{
  UBYTE i, script_cmd_index;
  SCRIPT_CMD_FN script_cmd_fn;

  if (!script_ptr_bank)
  {
    return;
  }

  script_cmd_index = ReadBankedUBYTE(script_ptr_bank, script_ptr);

  LOG("SCRIPT CMD INDEX WAS %u not=%u, zero=%u\n", script_cmd_index, !script_cmd_index, script_cmd_index == 0);

  if (!script_cmd_index)
  {
    LOG("SCRIPT FINISHED\n");
    script_ptr_bank = 0;
    script_ptr = 0;
    return;
  }

  script_cmd_args_len = script_cmds[script_cmd_index].args_len;
  script_cmd_fn = script_cmds[script_cmd_index].fn;

  LOG("SCRIPT cmd [%u - %u] = %u (%u)\n", script_ptr_bank, script_ptr, script_cmd_index, script_cmd_args_len);

  for (i = 0; i != script_cmd_args_len; i++)
  {
    script_cmd_args[i] = ReadBankedUBYTE(script_ptr_bank, script_ptr + i + 1);
    LOG("SCRIPT ARG-%u = %u\n", i, script_cmd_args[i]);
  }

  PUSH_BANK(scriptrunner_bank);
  script_cmd_fn();
  POP_BANK;

  last_fn = script_cmd_fn;

  if (script_continue)
  {
    LOG("CONTINUE!\n");
    ScriptRunnerUpdate();
  }
}
