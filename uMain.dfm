object frmMain: TfrmMain
  Left = 0
  Top = 0
  BorderWidth = 1
  Caption = 'LogicBuilder'
  ClientHeight = 381
  ClientWidth = 718
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = menuMain
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object pbMain: TPaintBox
    Left = 41
    Top = 0
    Width = 677
    Height = 381
    ParentCustomHint = False
    Align = alClient
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    ParentShowHint = False
    ShowHint = False
    OnMouseDown = pbMainMouseDown
    OnPaint = pbMainPaint
    ExplicitLeft = 45
  end
  object tbMain: TToolBar
    Left = 0
    Top = 0
    Width = 41
    Height = 381
    Align = alLeft
    ButtonWidth = 25
    EdgeBorders = [ebRight]
    Flat = False
    TabOrder = 3
    Transparent = True
    object ToolButton1: TToolButton
      Left = 0
      Top = 0
      Action = actTakeCursor
      Wrap = True
    end
    object ToolButton4: TToolButton
      Left = 0
      Top = 22
      Caption = 'ToolButton4'
      ImageIndex = 2
    end
  end
  object btnDbg: TButton
    Left = 73
    Top = 72
    Width = 75
    Height = 25
    Caption = 'btnDbg'
    TabOrder = 0
    OnClick = btnDbgClick
  end
  object memoDbg: TMemo
    Left = 73
    Top = 133
    Width = 75
    Height = 138
    TabOrder = 1
  end
  object lboxComponents: TListBox
    Left = 0
    Top = 72
    Width = 39
    Height = 289
    Style = lbOwnerDrawFixed
    BorderStyle = bsNone
    ItemHeight = 15
    Items.Strings = (
      'and2'
      'or2'
      'xor2'
      'nand2'
      'nor2'
      'nxor2'
      'and3'
      'or3'
      'xor3'
      'nand3'
      'nor3'
      'nxor3'
      'and4'
      'or4'
      'xor4'
      'nand4'
      'nor4'
      'nxor4'
      'not')
    TabOrder = 2
    OnDblClick = lboxComponentsDblClick
  end
  object menuMain: TMainMenu
    Left = 272
    Top = 208
    object File1: TMenuItem
      Caption = 'File'
      object Open1: TMenuItem
        Caption = 'Open..'
      end
      object Save1: TMenuItem
        Caption = 'Save'
      end
      object Saveas1: TMenuItem
        Caption = 'Save as..'
      end
      object Exit1: TMenuItem
        Caption = 'Exit'
      end
    end
    object Settings1: TMenuItem
      Caption = 'Settings'
    end
    object Help1: TMenuItem
      Caption = 'Help'
    end
  end
  object actlistMain: TActionList
    Left = 328
    Top = 216
    object actTakeCursor: TAction
      Category = 'Control'
      Caption = 'actTakeCursor'
      ShortCut = 67
      OnExecute = actTakeCursorExecute
    end
    object actMoveUp: TAction
      Category = 'Control'
      Caption = 'actMoveUp'
      ShortCut = 87
      OnExecute = actMoveUpExecute
    end
    object actMoveDown: TAction
      Category = 'Control'
      Caption = 'actMoveDown'
      ShortCut = 83
      OnExecute = actMoveDownExecute
    end
    object actMoveLeft: TAction
      Category = 'Control'
      Caption = 'actMoveLeft'
      ShortCut = 65
      OnExecute = actMoveLeftExecute
    end
    object actMoveRight: TAction
      Category = 'Control'
      Caption = 'actMoveRight'
      ShortCut = 68
      OnExecute = actMoveRightExecute
    end
    object actDeleteComponent: TAction
      Category = 'Control'
      Caption = 'actDeleteComponent'
      ShortCut = 46
      OnExecute = actDeleteComponentExecute
    end
    object actCheangeMoveStep: TAction
      Category = 'Control'
      Caption = 'actCheangeMoveStep'
      ShortCut = 8208
      OnExecute = actCheangeMoveStepExecute
    end
  end
end
