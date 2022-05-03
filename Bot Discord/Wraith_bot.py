import subprocess
from keep_alive import keep_alive
from os import getenv
from dotenv import load_dotenv
from typing import List, Optional, Union
import discord
from discord.ext import commands
import logging
from datetime import datetime, timedelta, timezone

load_dotenv()

# Logging everything in a file
logger: logging.Logger = logging.getLogger('discord')
logger.setLevel(logging.INFO)
handler: logging.FileHandler = logging.FileHandler(filename='discord.log', encoding='utf-8', mode='w')
handler.setFormatter(logging.Formatter('%(asctime)s:%(levelname)s:%(name)s: %(message)s'))
logger.addHandler(handler)

# Enabling proper intents so it's possible to get the members' list
intents: discord.flags.Intents = discord.Intents.default()
intents.members = True

# the Client - the user "account" representing the bot
client: commands.Bot = commands.Bot(command_prefix=["Wraith ", "wraith "],
                                    case_insensitive=True,
                                    activity=discord.Activity(type=discord.ActivityType.watching, name='the chat'),
                                    intents=intents,
                                    help_command=None)

# Setup
ds_server: int = 729353421483933716  # Dark Souls Community
my_server: int = 690212035757211663  # ShadowHunter's Server
k: int = 0


# "Listening" so it doesn't override the default method
@client.listen()
# When the bot is ready to use, it will display the message with its name and set activity to "watching the chat"
async def on_ready():
    print('I am logged in as {0.user}'.format(client))


# Help command
@client.command()
async def help(ctx: commands.context.Context, arg: Optional[str]):
    dm: bool = False
    # Checking whether help should be sent directly or in a public channel
    if arg and arg.lower() == "dm":
        dm = True
    # Non-breaking space - normal spaces and Tabs aren't working in an embed
    nbsp: str = "\xa0"
    help_msg: str = "Use `Wraith seek (<name>) (-all) (-roles)` so I will find the illiterate\n" \
                    f"{10 * nbsp}optional arguments:\n" \
                    f"{20 * nbsp}`<name>` - which server are we even talking about? (if used in DM)\n" \
                    f"{20 * nbsp}`-all` - doesn't matter when they joined\n" \
                    f"{20 * nbsp}`-roles` - what roles they DID take?\n" \
                    "Use `Wraith help`... I think that's obvious, actually...\n" \
                    f"{10*nbsp} use `Wraith help dm` to get it sent directly to you\n" \
                    "Use `Wraith ping` to play ping pong\n" \
                    "Use `Wraith when <User_ID>` to see how long do they stay here\n" \
                    "Use `PraiseTheSun` to PRAISE THE SUN!\n" \
                    "Use `Wraith bearer` to s-*skipped*"
    # If it's the owner, send him more commands privately
    if dm and commands.is_owner():
        help_msg += "\nUse `Wraith save_log` to save the discord.log to a new file\n" \
                    "Use `Wraith server_info <name>` to get basic info about the server"
    # Creating an embed
    embed: discord.Embed = discord.Embed(description=help_msg)
    # Author's name is just so the description shows up nicely
    embed.set_author(name="Guide", icon_url=client.user.avatar_url)
    # If the "dm" argument was used, try sending the help directly
    if dm:
        try:
            await ctx.author.send(embed=embed)
        # Direct messages may be turned off by the user
        except discord.Forbidden:
            await ctx.send("If only I could DM you...")
    else:
        try:
            await ctx.send(embed=embed)
        # Might not have permissions to send an embed
        except discord.Forbidden:
            help_msg = "____________________________________________________________" + help_msg + \
                       "____________________________________________________________"
            await ctx.send(help_msg)


# Ping
@client.command()
async def ping(ctx: commands.context.Context):
    await ctx.send(f"{ctx.author.mention} pong")


# Easter Egg
@client.command()
async def bearer(ctx: commands.context.Context):
    global k
    text: str
    k += 1
    if k < 3:
        text = "Bearer...\n" \
               "Seek...\n" \
               "Seek...\n" \
               "Lest..."
    else:
        text = "Bearer of the curse...\n" \
               "Seek souls. Larger, more powerful souls.\n" \
               "Seek the King, that is the only way.\n" \
               "Lest this land swallow you whole... As it has so many others."
        k = 0
    await ctx.send(text)


# Easter egg #2
@client.command()
async def PraiseTheSun(ctx: commands.context.Context):
    for emoji in client.emojis:
        if emoji.name == "Solaire_Praise_The_Sun":
            await ctx.send(str(emoji))
            return


# How long someone has been on the server, returned as a number of seconds or as a timedelta object
def user_time_on_the_server(user: discord.Member, timestamp=False) -> Union[float, timedelta]:
    if timestamp:
        return abs(datetime.now(timezone.utc).timestamp() - user.joined_at.timestamp())
    else:
        return abs(datetime.utcnow() - user.joined_at)


# Shows how long someone has been in the server
@client.command()
async def when(ctx: commands.context.Context, user_id: int):
    if type(ctx.channel) != discord.TextChannel:
        await ctx.send("This is not a server...")
        return
    member: discord.Member = ctx.guild.get_member(user_id)
    delta: timedelta = user_time_on_the_server(member)
    hours: int = delta.seconds // 3600
    msg: str = f"User {member.__str__()} has last joined the server " \
               f"{delta.days} days, {hours} hours and {delta.seconds // 60 - 60 * hours} minutes ago"
    await ctx.send(msg)


# Basic information about the server
@commands.is_owner()
@client.command()
async def server_info(ctx: commands.context.Context, *, server_name: str):
    for guild in client.guilds:
        if guild.name == server_name:
            await ctx.send(f"Server name: {guild.name}, id: {guild.id}, owner: {guild.owner.__str__()}")
            return
    await ctx.send("First bring me with you to that server")


# Seeking out members
@client.command()
async def seek(ctx: commands.context.Context, *, server_name_and_optional_args: Optional[str] = None):
    # Whether to only take into account users who have already been in the server for 2 hours
    time_limit: bool = True
    # Whether to also send the users' roles, not just their names and IDs
    add_roles: bool = False
    # Checking if there are any optional arguments
    if server_name_and_optional_args:
        args: List[str] = server_name_and_optional_args.split()
        if "-all" in args:
            time_limit = False
            args.remove("-all")
        if "-roles" in args:
            add_roles = True
            args.remove("-roles")
        # Checking if there is a server's name or if it were just other arguments
        server_name_and_optional_args = " ".join(args)
        if server_name_and_optional_args == "":
            server_name_and_optional_args = None
    # Checking if it's called in a server, if there is no name
    if server_name_and_optional_args is None and type(ctx.channel) != discord.TextChannel:
        await ctx.send("This is not a server...")
        return
    server: Optional[discord.Guild] = None
    if server_name_and_optional_args:
        # Trying to find the server in bot's servers
        for guild in client.guilds:
            if guild.name == server_name_and_optional_args:
                server = guild
                break
        if not server:
            await ctx.send("First bring me with you to that server")
            return
        # Not allowing to use the seek command on other servers
        elif type(ctx.channel) == discord.TextChannel and ctx.guild != server:
            await ctx.send("Do you want to moderate another server?")
            return
    else:
        server = ctx.guild
    members: List[discord.Member] = server.members
    obligatory_role: int
    platform: List[int]
    game: List[int]
    # Choosing which set of roles to take into account (dependant on the server)
    if server.id == my_server:
        # Example Roles - my own server
        excluded_role = 0
        obligatory_role = 958343539471839302  # Chosen Undead
        platform = [958347227724673025, 958347262579310682]  # PC, PS4
        game = [958347158384418846, 958347198825889822]  # DS1, DS2
    elif server.id == ds_server:
        # DS Community
        excluded_role = 729360984145133738  # NPCs
        obligatory_role = 729360164410359849  # Chosen Undead
        platform = [729709269766635541,  # PC
                    729709558531883079, 729709416164622408, 779067517662527499,  # PS3, PS4, PS5
                    729709619886162052, 729709751088447499, 790239600534618115,  # XBOX 360, XBOX 1, XBOX Series X
                    729709874128355469]  # SWITCH
        game = [732869956592467969,  # Demon Souls
                732869396556677160, 732869473949843486,  # DS1, DSR
                732869519504179323, 732869603914547300,  # DS2, DS3
                732869689071370251, 944517370985193532]  # Bloodborne, Elden Ring
    else:
        await ctx.send("My apologies, something went wrong...")
        raise ValueError("Error in the chosen server's name - look in setup")
    # The "seeking"
    found: str = ""
    for member in members:
        delta: float = user_time_on_the_server(member, timestamp=True)
        # Omit users who have joined less than 2 hours ago
        if time_limit and abs(delta) < 7200:
            continue
        # User's roles, by ID
        roles: List[int] = [role.id for role in member.roles]
        if excluded_role in roles or member.id == 860022391786700820:  # Greetings Ed
            continue
        # Checking if a member has the obligatory role, as well as a platform and a game role ->
        #                                                           by using the sets' intersections
        if obligatory_role not in roles or \
                len(set(platform).intersection(set(roles))) == 0 or \
                len(set(game).intersection(set(roles))) == 0:
            found += f"{str(member)}, {member.id}"
            if add_roles:
                found += f" - roles: {','.join([role.name for role in member.roles])}\n"
            else:
                found += "\n"
    if len(found) == 0:
        await ctx.send("No more victims...")
    else:
        await ctx.send(found)


# Logs
def _save_log(filename_to_save: str):
    with open("discord.log", 'r') as f:
        with open(filename_to_save, "w") as f_new:
            lines: List[str] = f.readlines()
            f_new.writelines(lines)
            f_new.close()
        f.close()


@client.command()
@commands.is_owner()
async def save_log(ctx: commands.context.Context):
    _save_log("saved_log.txt")
    await ctx.send("Log saved")


def save_emergency_log():
    _save_log("Emergency_exit.log")


# # When someone sends a message
# @client.listen()
# async def on_message(message: discord.Message):
#     # Ignore message sent by self
#     if message.author == client.user:
#         return
#     msg: str = message.content
#     channel: discord.channel = message.channel


if __name__ == '__main__':
    # Running the bot and magically keeping it alive
    keep_alive()
    try:
        client.run(getenv('TOKEN'))
    # If the shared IP of replit is rate-limited
    except discord.HTTPException:
        print("HTTPException detected!")
        save_emergency_log()
        print("Log saved!")
        subprocess.run(["kill", 1], capture_output=True)
        print("Did it work?")
