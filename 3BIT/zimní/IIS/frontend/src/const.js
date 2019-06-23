// API cesty
export const API_BASE = 'http://localhost:5000/'
export const ROUTE_PREFIX = '/'
export const API_MAIN = `${API_BASE}tea/`
export const TOKEN_NAME = 'caje113kLoginToken'
export const BASKET_NAME = 'caje113kBasket'
export const LOG_IN = 'login'
export const LOG_OUT = 'logout'

// Header
export const HEADER_STORE = 'headerStore'
export const HEADER_USER = 'headerUser'

// Barvy kategorii
export const CATEGORIES = Object.freeze({
	oolongy: {
		'fullName': 'Oolongy',
		'route': 'oolongy'
	},
	puerh: {
		'fullName': 'Puerh',
		'route': 'puerh'
	},
	green: {
		'fullName': 'Zelené',
		'route': 'zelene'
	},
	white: {
		'fullName': 'Bílé',
		'route': 'white'
	},
	black : {
		'fullName': 'Černé',
		'route': 'black'
	},
	yellow : {
		'fullName': 'Žluté',
		'route': 'zlute'
	}
})

export function encodeEmailHash(email) {
	return window.btoa(email)
}

export function decodeEmailHash(hash) {
	return window.atob(hash)
}

export function getAuthorities() {
	return [
		{
			no: 1,
			name: 'retail',
			fullName: 'Maloobchodník'
		},
		{
			no: 2,
			name: 'wholesale',
			fullName: 'Velkoobchodník'
		},
		{
			no: 3,
			name: 'manager',
			fullName: 'Manažer'
		},
		{
			no: 5,
			name: 'admin',
			fullName: 'Správce'
		}
	]
}

export function on403() {
	window.location.href = `${ROUTE_PREFIX}logout`
}

export function getUser() {
	return JSON.parse(localStorage.getItem(TOKEN_NAME))
}

export function removeUser() {

}



// function findTeaInBasket() {

// }

// function findBatchInBasket() {

// }


export function addToBasket(item, email) {
	localStorage.setItem(BASKET_NAME, JSON.stringify({basketId: item, email: email}))
}

export function getBasketId() {
	return localStorage.getItem(BASKET_NAME)
}

// export function unsetUser() {
	
// }