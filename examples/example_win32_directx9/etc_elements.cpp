#include "etc_elements.h"

bool e_elements::begin_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags window_flags)
{
  ImGuiChildFlags child_flags = ImGuiChildFlags_None;
  ImGuiContext& g = *GImGui;
  ImGuiWindow* parent_window = g.CurrentWindow;
  IM_ASSERT(id != 0);

  // Sanity check as it is likely that some user will accidentally pass ImGuiWindowFlags into the ImGuiChildFlags argument.
  const ImGuiChildFlags ImGuiChildFlags_SupportedMask_ = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_FrameStyle | ImGuiChildFlags_NavFlattened;
  IM_UNUSED(ImGuiChildFlags_SupportedMask_);
  IM_ASSERT((child_flags & ~ImGuiChildFlags_SupportedMask_) == 0 && "Illegal ImGuiChildFlags value. Did you pass ImGuiWindowFlags values instead of ImGuiChildFlags?");
  IM_ASSERT((window_flags & ImGuiWindowFlags_AlwaysAutoResize) == 0 && "Cannot specify ImGuiWindowFlags_AlwaysAutoResize for BeginChild(). Use ImGuiChildFlags_AlwaysAutoResize!");
  if (child_flags & ImGuiChildFlags_AlwaysAutoResize)
  {
    IM_ASSERT((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0 && "Cannot use ImGuiChildFlags_ResizeX or ImGuiChildFlags_ResizeY with ImGuiChildFlags_AlwaysAutoResize!");
    IM_ASSERT((child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) != 0 && "Must use ImGuiChildFlags_AutoResizeX or ImGuiChildFlags_AutoResizeY with ImGuiChildFlags_AlwaysAutoResize!");
  }
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
  if (window_flags & ImGuiWindowFlags_AlwaysUseWindowPadding)
    child_flags |= ImGuiChildFlags_AlwaysUseWindowPadding;
  if (window_flags & ImGuiWindowFlags_NavFlattened)
    child_flags |= ImGuiChildFlags_NavFlattened;
#endif
  if (child_flags & ImGuiChildFlags_AutoResizeX)
    child_flags &= ~ImGuiChildFlags_ResizeX;
  if (child_flags & ImGuiChildFlags_AutoResizeY)
    child_flags &= ~ImGuiChildFlags_ResizeY;

  // Set window flags
  window_flags |= ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoTitleBar;
  window_flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove); // Inherit the NoMove flag
  if (child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize))
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
  if ((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0)
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

  // Special framed style
  if (child_flags & ImGuiChildFlags_FrameStyle)
  {
    PushStyleColor(ImGuiCol_ChildBg, g.Style.Colors[ImGuiCol_FrameBg]);
    PushStyleVar(ImGuiStyleVar_ChildRounding, g.Style.FrameRounding);
    PushStyleVar(ImGuiStyleVar_ChildBorderSize, g.Style.FrameBorderSize);
    PushStyleVar(ImGuiStyleVar_WindowPadding, g.Style.FramePadding);
    child_flags |= ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;
    window_flags |= ImGuiWindowFlags_NoMove;
  }

  // Forward size
  // Important: Begin() has special processing to switch condition to ImGuiCond_FirstUseEver for a given axis when ImGuiChildFlags_ResizeXXX is set.
  // (the alternative would to store conditional flags per axis, which is possible but more code)
  const ImVec2 size_avail = GetContentRegionAvail();
  const ImVec2 size_default((child_flags & ImGuiChildFlags_AutoResizeX) ? 0.0f : size_avail.x, (child_flags & ImGuiChildFlags_AutoResizeY) ? 0.0f : size_avail.y);
  ImVec2 size = CalcItemSize(size_arg, size_default.x, size_default.y);

  // A SetNextWindowSize() call always has priority (#8020)
  // (since the code in Begin() never supported SizeVal==0.0f aka auto-resize via SetNextWindowSize() call, we don't support it here for now)
  // FIXME: We only support ImGuiCond_Always in this path. Supporting other paths would requires to obtain window pointer.
  if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) != 0 && (g.NextWindowData.SizeCond & ImGuiCond_Always) != 0)
  {
    if (g.NextWindowData.SizeVal.x > 0.0f)
    {
      size.x = g.NextWindowData.SizeVal.x;
      child_flags &= ~ImGuiChildFlags_ResizeX;
    }
    if (g.NextWindowData.SizeVal.y > 0.0f)
    {
      size.y = g.NextWindowData.SizeVal.y;
      child_flags &= ~ImGuiChildFlags_ResizeY;
    }
  }
  SetNextWindowPos(ImVec2(parent_window->DC.CursorPos.x, parent_window->DC.CursorPos.y + 34.0f));
  SetNextWindowSize(size);

  parent_window->DrawList->AddRectFilled(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, size.y), ImColor(24, 24, 26), 4.0f);
  parent_window->DrawList->AddRect(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, size.y), ImColor(1.0f, 1.0f, 1.0f, 0.03f), 4.0f);
  parent_window->DrawList->AddLine(parent_window->DC.CursorPos + ImVec2(1, 32), parent_window->DC.CursorPos + ImVec2(size.x - 1, 32), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
  parent_window->DrawList->AddRectFilledMultiColorRounded(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x - 115, 32), ImColor(24, 24, 26), ImColor(147, 190, 66, 50), ImColor(147, 190, 66, 0), ImColor(147, 190, 66, 0), ImColor(147, 190, 66, 50), 4.0f, ImDrawCornerFlags_TopLeft);
  parent_window->DrawList->AddText(bold, 15.0f, parent_window->DC.CursorPos + ImVec2(16, 9), ImColor(147, 190, 66), name);


  // Forward child flags
  g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasChildFlags;
  g.NextWindowData.ChildFlags = child_flags;

  // Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
  // FIXME: 2023/11/14: commented out shorted version. We had an issue with multiple ### in child window path names, which the trailing hash helped workaround.
  // e.g. "ParentName###ParentIdentifier/ChildName###ChildIdentifier" would get hashed incorrectly by ImHashStr(), trailing _%08X somehow fixes it.
  const char* temp_window_name;
  /*if (name && parent_window->IDStack.back() == parent_window->ID)
      ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s", parent_window->Name, name); // May omit ID if in root of ID stack
  else*/
  if (name)
    ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
  else
    ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

  // Set style
  const float backup_border_size = g.Style.ChildBorderSize;
  if ((child_flags & ImGuiChildFlags_Borders) == 0)
    g.Style.ChildBorderSize = 0.0f;

  // Begin into window
  const bool ret = Begin(temp_window_name, NULL, window_flags);

  // Restore style
  g.Style.ChildBorderSize = backup_border_size;
  if (child_flags & ImGuiChildFlags_FrameStyle)
  {
    PopStyleVar(3);
    PopStyleColor();
  }

  ImGuiWindow* child_window = g.CurrentWindow;
  child_window->ChildId = id;

  // Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
  // While this is not really documented/defined, it seems that the expected thing to do.
  if (child_window->BeginCount == 1)
    parent_window->DC.CursorPos = child_window->Pos;

  // Process navigation-in immediately so NavInit can run on first frame
  // Can enter a child if (A) it has navigable items or (B) it can be scrolled.
  const ImGuiID temp_id_for_activation = ImHashStr("##Child", 0, id);
  if (g.ActiveId == temp_id_for_activation)
    ClearActiveID();
  if (g.NavActivateId == id && !(child_flags & ImGuiChildFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
  {
    FocusWindow(child_window);
    NavInitWindow(child_window, false);
    SetActiveID(temp_id_for_activation, child_window); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
    g.ActiveIdSource = g.NavInputSource;
  }
  return ret;
}

bool e_elements::begin_child(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 11));
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
    return begin_child_ex(str_id, window->GetID(str_id), size_arg, border, extra_flags | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove);
}

bool e_elements::begin_child(ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
{
    IM_ASSERT(id != 0);
    return begin_child_ex(NULL, id, size_arg, border, extra_flags);
}

void e_elements::end_child()
{
  ImGuiContext& g = *GImGui;
  ImGuiWindow* child_window = g.CurrentWindow;

  IM_ASSERT(g.WithinEndChild == false);
  IM_ASSERT(child_window->Flags & ImGuiWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() calls

  g.WithinEndChild = true;
  ImVec2 child_size = child_window->Size;
  End();
  if (child_window->BeginCount == 1)
  {
    ImGuiWindow* parent_window = g.CurrentWindow;
    ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + child_size);
    ItemSize(child_size);
    const bool nav_flattened = (child_window->ChildFlags & ImGuiChildFlags_NavFlattened) != 0;
    if ((child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY) && !nav_flattened)
    {
      ItemAdd(bb, child_window->ChildId);
      RenderNavHighlight(bb, child_window->ChildId);

      // When browsing a window that has no activable items (scroll only) we keep a highlight on the child (pass g.NavId to trick into always displaying)
      if (child_window->DC.NavLayersActiveMask == 0 && child_window == g.NavWindow)
        RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_Compact);
    }
    else
    {
      // Not navigable into
      // - This is a bit of a fringe use case, mostly useful for undecorated, non-scrolling contents childs, or empty childs.
      // - We could later decide to not apply this path if ImGuiChildFlags_FrameStyle or ImGuiChildFlags_Borders is set.
      ItemAdd(bb, child_window->ChildId, NULL, ImGuiItemFlags_NoNav);

      // But when flattened we directly reach items, adjust active layer mask accordingly
      if (nav_flattened)
        parent_window->DC.NavLayersActiveMaskNext |= child_window->DC.NavLayersActiveMaskNext;
    }
    if (g.HoveredWindow == child_window)
      g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
  }
  ImGui::PopStyleVar(2);
  g.WithinEndChild = false;
  g.LogLinePosY = -FLT_MAX; // To enforce a carriage return
}
