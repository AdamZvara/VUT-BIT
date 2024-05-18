
enum UserRoleEnum {
  ADMIN = 0,
  MOD = 1,
  USER = 2,
}

export function getRoleText(role: number) {
  if (role == UserRoleEnum.ADMIN) {
    return "administrátor";
  } else if (role == UserRoleEnum.USER) {
    return "užívatel";
  } else {
    return "moderátor";
  }
}

export function getRoleColor(role: number) {
  if (role == UserRoleEnum.ADMIN) {
    return "secondary";
  } else if (role == UserRoleEnum.USER) {
    return "primary";
  } else {
    return "alert";
  }
}

export default UserRoleEnum;