
import pygame
import validators
import  commands
from UI.constants import Width,Height,ICON_TYPES,RUNSYS
from UI.simple_name_space import SimpleNamespace
from UI.page  import Page
from UI.label  import Label
from UI.icon_item import IconItem
from UI.keys_def  import CurKeys, IsKeyMenuOrB, IsKeyStartOrA
from UI.skin_manager import MySkinManager
from UI.lang_manager import MyLangManager

from libs.DBUS  import is_wifi_connected_now,get_wifi_ip
from collections import deque
from enum import Enum
import os
import subprocess
import pipes

class DevilutionPage(Page):
	_FootMsg = ["Nav","Check","Upgrade","Back","Play"]

	_GameName = "devilutionX"
	_GamePath = "/home/cpi/games/devilutionX"
	_GameExecutable = _GamePath + "/build/devilutionx"
	_GameExecutableRevision = _GameExecutable + ".rev"
	_GameBuildScript = _GamePath + "/Packaging/cpi-gamesh/build.sh -t " + pipes.quote(os.path.dirname(os.path.abspath( __file__ )))
	_GamePNG = _GamePath + "/Packaging/cpi-gamesh/Devilution.png"
	_DevilutionDiabdatmpq = "/home/cpi/.local/share/diasurgical/devilution/diabdat.mpq"
	_DevilutionDiabdatmpqPresent = False
	_GameInstalled = False

	_CiteNewUpdate = "Ahh... fresh meat!"
	_CiteCheckUpdate = "Lets search the books..."
	_CiteWelcome = "Well, what can I do for ya?"
	_CiteCompiling = "Stay awhile and listen."
	_CiteDone = "You must venture through the portal..."
	_CiteFailed = "Game Over. Better luck next time!"

	_GitURL = "https://github.com/diasurgical/devilutionX.git"
	_GitBranch = "master"
	_GitRevision = ""

	_GameIcon = None

	_Process = None
	_Labels = {}
	_Coords = {}

	_ListFontObj = MyLangManager.TrFont("varela13")
	_URLColor  = MySkinManager.GiveColor('URL')
	_TextColor = MySkinManager.GiveColor('Text')

	def __init__(self):
		Page.__init__(self)

	def InitLabels(self):

		y = 15
		x = 11
		yInc = 19
		xGitRefLabelWidth = 48

		labels = \
			[["greeting",self._CiteWelcome, self._TextColor, x, y],
			 ["status", "", self._URLColor, x, y + 72-yInc],
			 ["comment", "", self._TextColor, x, y + 72],
			 ["console_out","",self._URLColor, x, y + 72 + yInc],
			 ["label_rev","GIT Revisions: ", self._TextColor, x, 132],
			 ["label_git_rev","Source: ", self._TextColor, x, 151],
			 ["content_git_rev","", self._URLColor, x + xGitRefLabelWidth, 151],
			 ["label_bin_rev","Bin: ", self._TextColor, x, 170],
			 ["content_bin_rev","", self._URLColor, x + xGitRefLabelWidth, 170]
			]

		for i in labels:
			l = Label()
			l.SetCanvasHWND(self._CanvasHWND)
			l.Init(i[1],self._ListFontObj)
			l.SetColor(i[2])
			self._Labels[ i[0] ] = l

			c = SimpleNamespace()
			c.x = i[3]
			c.y = i[4]
			self._Coords[ i[0] ] = c

	def GitGetRevision(self):
		if not os.path.exists(self._GamePath):
			return "game not installed"
		process = subprocess.Popen("cd " + pipes.quote(self._GamePath) + "; git rev-parse HEAD",stdout=subprocess.PIPE,stderr=subprocess.STDOUT, shell=True)
		self._GitRevision = process.communicate()[0].strip()
		process.wait()
		return self._GitRevision

	def ExectuableGetRevision(self):
		try:
			with open(self._GameExecutableRevision, 'r') as file:
				executableRevsion = file.read().replace('\n', '')
			return executableRevsion
		except:
			return "unknown"

	def InitGameDirectory(self):
		try:
			os.makedirs(self._GamePath)
		except:
			pass
		self.StartShellProcess("cd " + pipes.quote(self._GamePath) + "; git clone --single-branch --branch " + pipes.quote(self._GitBranch) + " " + pipes.quote(self._GitURL) + " .")

	def CheckDevilutionMPQ(self):
		self._DevilutionDiabdatmpqPresent = os.path.isfile(self._DevilutionDiabdatmpq)

	def CheckGameInstalled(self):
		self._GameInstalled = os.path.isfile(self._GameExecutable)

	def UpdateFootMsg(self):
		if not self._GameInstalled:
			self._FootMsg = ["Nav","","Install","Back",""]
			self.UpdateLabel("status", "GIT Upgrade")
			self.UpdateLabel("comment", "Press X to install")
		elif not self._DevilutionDiabdatmpqPresent:
			self._FootMsg = ["Nav","","Upgrade","Back","Re-check"]
			self.UpdateLabel("status", "Gamefile diabdat.mpq missing")
			self.UpdateLabel("comment", "see readme")
		else:
			self._FootMsg = ["Nav","","Upgrade","Back","Play"]
			self.UpdateLabel("status", "Ready")
			self.UpdateLabel("comment", self._CiteDone)

	def Init(self):
		Page.Init(self)

		if self._Screen != None:
			if self._Screen._CanvasHWND != None and self._CanvasHWND == None:
				self._HWND = self._Screen._CanvasHWND
				self._CanvasHWND = pygame.Surface( (self._Screen._Width,self._Screen._Height) )

		if os.path.isfile(self._GamePNG):
			self._GameIcon = IconItem()
			self._GameIcon._ImageName = self._GamePNG
			self._GameIcon._MyType = ICON_TYPES["STAT"]
			self._GameIcon._Parent = self
			self._GameIcon.Adjust(290,70,128,128,0)

		self.InitLabels()
		self.CheckDevilutionMPQ()
		self.CheckGameInstalled()
		self.UpdateFootMsg()

		self.UpdateLabel("content_git_rev", self.GitGetRevision(), 24)
		self.UpdateLabel("content_bin_rev", self.ExectuableGetRevision(), 24)

	def UpdateLabel(self, label, msg, maxLen=38):
		print(label + ": " + msg)
		if len(msg) > maxLen:
			m = msg[:maxLen] + "..."
		else:
			m = msg
		self._Labels[label].SetText(m)

	def StartShellProcess(self, cmd):
		print("StartShellProcess " + cmd)
		proc = subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT, shell=True)
		while(True):
			line = proc.stdout.readline()
			if line:
				self.UpdateLabel("console_out", line.strip(), 48)
				self._Screen.Draw()
				self._Screen.SwapAndShow()
			if line == '' and proc.poll() is not None:
				break
		self.UpdateLabel("console_out", "")
		self._Screen.Draw()
		self._Screen.SwapAndShow()

	def GitUpgrade(self):
		self.UpdateLabel("status", "GIT Upgrade")
		self.UpdateLabel("comment", self._CiteCheckUpdate)

		curRev = "unset"
		if not os.path.exists(self._GamePath):
			self.InitGameDirectory()
		else:
			curRev = self.GitGetRevision()
			self.StartShellProcess("cd " + pipes.quote(self._GamePath) + "; git pull")
		
		self._GitRevision = self.GitGetRevision()
		self.UpdateLabel("content_git_rev", self._GitRevision, 24)

		if curRev != self._GitRevision:
			self.UpdateLabel("comment", self._CiteNewUpdate)
		else:
			self.UpdateLabel("comment", self._CiteDone)

		self._Screen.Draw()
		self._Screen.SwapAndShow()

	def GitExectuableIsGitRevision(self):
		return self.GitGetRevision() == self.ExectuableGetRevision()

	def Build(self):
		self.UpdateLabel("status", "Building")
		self.StartShellProcess(self._GameBuildScript)

	def UpgradeAndBuild(self):
		self.GitUpgrade()
		self.UpdateLabel("comment", self._CiteCompiling)
		self._Screen.Draw()
		self._Screen.SwapAndShow()
		if not self.GitExectuableIsGitRevision():
			self.Build()

		self.UpdateLabel("content_git_rev", self.GitGetRevision(), 24)
		self.UpdateLabel("content_bin_rev", self.ExectuableGetRevision(), 24)

		self.UpdateLabel("status", "Done")
		if self.GitExectuableIsGitRevision():
			self.UpdateLabel("comment", self._CiteDone)
		else:
			self.UpdateLabel("comment", self._CiteFailed)

		self.CheckDevilutionMPQ()
		self.CheckGameInstalled()
		self.UpdateFootMsg()
		
		self._Screen.Draw()
		self._Screen.SwapAndShow()

	def KeyDown(self,event):

		if IsKeyMenuOrB(event.key):
			self.ReturnToUpLevelPage()
			self._Screen.Draw()
			self._Screen.SwapAndShow()

		if self._DevilutionDiabdatmpqPresent and self._GameInstalled:
			if IsKeyStartOrA(event.key):
				pygame.event.post( pygame.event.Event(RUNSYS, message=self._GameExecutable))
			if event.key == CurKeys["X"]:
				self.UpgradeAndBuild()
		elif not self._GameInstalled:
			if event.key == CurKeys["X"]:
				self.UpgradeAndBuild()
		elif not self._DevilutionDiabdatmpqPresent:
			if IsKeyStartOrA(event.key):
				self.CheckDevilutionMPQ()
				self.CheckGameInstalled()
				self.UpdateFootMsg()
				self._Screen.Draw()
				self._Screen.SwapAndShow()

	def Draw(self):
		self.ClearCanvas()

		if self._GameIcon != None:
			self._GameIcon.Draw()

		for i in self._Labels:
			if i in self._Coords:
				self._Labels[i].NewCoord( self._Coords[i].x, self._Coords[i].y)
				self._Labels[i].Draw()
			
		if self._HWND != None:
			self._HWND.fill(MySkinManager.GiveColor('White'))
			self._HWND.blit(self._CanvasHWND,(self._PosX,self._PosY,self._Width, self._Height ) )

		
class APIOBJ(object):

	_Page = None
	def __init__(self):
		pass
	def Init(self,main_screen):
		self._Page = DevilutionPage()
		self._Page._Screen = main_screen
		self._Page._Name ="devilutionX"
		self._Page.Init()
		
	def API(self,main_screen):
		if main_screen !=None:
			main_screen.PushPage(self._Page)
			main_screen.Draw()
			main_screen.SwapAndShow()

OBJ = APIOBJ()
def Init(main_screen):    
	OBJ.Init(main_screen)
def API(main_screen):
	OBJ.API(main_screen)
